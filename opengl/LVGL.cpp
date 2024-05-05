/*
 * LVGL for DPF
 * Copyright (C) 2024 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "LVGL.hpp"
#include "OpenGL.hpp"

#include "../distrho/extra/Sleep.hpp"
#include "../distrho/extra/Time.hpp"

#include "demos/lv_demos.h"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

static thread_local lv_global_t* lv_global = nullptr;

static uint32_t gettime_ms() noexcept
{
    return DISTRHO_NAMESPACE::d_gettime_ms();
}

struct LVGLWidget::PrivateData {
    LVGLWidget* const self;
    lv_global_t* const global;

    GLuint textureId = 0;
    Size<uint> textureSize;
    void* textureData = nullptr;

    lv_display_t* display = nullptr;

    explicit PrivateData(LVGLWidget* const s)
        : self(s),
          global(static_cast<lv_global_t*>(std::calloc(1, sizeof(lv_global_t))))
    {
        lv_global = global;
        init();
    }

    ~PrivateData()
    {
        lv_global = global;
        cleanup();
        lv_global = nullptr;
        std::free(global);
    }

private:
    void init()
    {
        lv_init();
        lv_delay_set_cb(d_msleep);
        lv_tick_set_cb(gettime_ms);

        const uint width = self->getWidth() ?: 2000;
        const uint height = self->getHeight() ?: 1200;

        display = lv_display_create(width, height);
        DISTRHO_SAFE_ASSERT_RETURN(display != nullptr,);

        lv_display_set_dpi(display, LV_DPI_DEF * self->getScaleFactor());

        glGenTextures(1, &textureId);
        DISTRHO_SAFE_ASSERT_RETURN(textureId != 0,);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        static constexpr const float transparent[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, transparent);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        lv_display_set_driver_data(display, this);
        lv_display_set_flush_cb(display, flush_cb);
        lv_display_add_event_cb(display, resolution_changed_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

        recreateTextureData(width, height);
    }

    void cleanup()
    {
        global->deinit_in_progress = true;

        if (display != nullptr)
        {
            lv_display_delete(display);
            display = nullptr;
        }

        if (textureId != 0)
        {
            glDeleteTextures(1, &textureId);
            textureId = 0;
        }

        std::free(textureData);
        textureData = nullptr;
    
        lv_deinit();
    }

    void recreateTextureData(const uint width, const uint height)
    {
        const auto format = lv_display_get_color_format(display);
        const uint32_t stride = lv_draw_buf_width_to_stride(width, format);
        const uint32_t data_size = height * stride;

        textureData = std::realloc(textureData, data_size);
        std::memset(textureData, 0, data_size);

        textureSize = Size<uint>(width, height);
        lv_display_set_buffers(display, textureData, nullptr, data_size, LV_DISPLAY_RENDER_MODE_DIRECT);
    }

    static void resolution_changed_cb(lv_event_t* const ev)
    {
        lv_display_t* const evdisplay = static_cast<lv_display_t*>(lv_event_get_current_target(ev));
        PrivateData* const evthis = static_cast<PrivateData*>(lv_display_get_driver_data(evdisplay));
        const uint width = lv_display_get_horizontal_resolution(evdisplay);
        const uint height = lv_display_get_vertical_resolution(evdisplay);

        evthis->recreateTextureData(width, height);
    }

    static void flush_cb(lv_display_t* const evdisplay, const lv_area_t*, uint8_t*)
    {
        lv_display_flush_ready(evdisplay);
    }

    DISTRHO_DECLARE_NON_COPYABLE(PrivateData)
};

// --------------------------------------------------------------------------------------------------------------------

LVGLWidget::LVGLWidget(TopLevelWidget* const tlw)
    : TopLevelWidget(tlw->getWindow()),
      lvglData(new PrivateData(this))
{
    addIdleCallback(this, 1000 / 60); // 60 fps

    // TESTING
    lv_demo_music();
}

LVGLWidget::LVGLWidget(Window& windowToMapTo)
    : TopLevelWidget(windowToMapTo),
      lvglData(new PrivateData(this))
{
    addIdleCallback(this, 1000 / 60); // 60 fps

    // TESTING
    lv_demo_music();
}

LVGLWidget::~LVGLWidget()
{
    removeIdleCallback(this);
    delete lvglData;
}

void LVGLWidget::idleCallback()
{
    lv_global = lvglData->global;
    lv_timer_handler();

    // FIXME
    TopLevelWidget::repaint();
}

void LVGLWidget::onDisplay()
{
    DISTRHO_SAFE_ASSERT_RETURN(getSize() == lvglData->textureSize,);

   #if LV_COLOR_DEPTH == 32
    constexpr const GLenum format = GL_BGRA;
   #elif LV_COLOR_DEPTH == 24
    constexpr const GLenum format = GL_BGR;
   #else
    #error Unsupported color format
   #endif

    const int width = static_cast<int>(getWidth());
    const int height = static_cast<int>(getHeight());

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lvglData->textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, lvglData->textureData);

    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.0f, 0.0f);
        glVertex2d(0, 0);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2d(width, 0);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2d(width, height);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2d(0, height);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

bool LVGLWidget::onKeyboard(const Widget::KeyboardEvent& event)
{
    if (TopLevelWidget::onKeyboard(event))
        return true;

    // TODO
    return false;
}

bool LVGLWidget::onCharacterInput(const Widget::CharacterInputEvent& event)
{
    if (TopLevelWidget::onCharacterInput(event))
        return true;

    // TODO
    return false;
}

bool LVGLWidget::onMouse(const Widget::MouseEvent& event)
{
    if (TopLevelWidget::onMouse(event))
        return true;

    // TODO
    return false;
}

bool LVGLWidget::onMotion(const Widget::MotionEvent& event)
{
    if (TopLevelWidget::onMotion(event))
        return true;

    // TODO
    return false;
}

bool LVGLWidget::onScroll(const Widget::ScrollEvent& event)
{
    if (TopLevelWidget::onScroll(event))
        return true;

    // TODO
    return false;
}

void LVGLWidget::onResize(const Widget::ResizeEvent& event)
{
    TopLevelWidget::onResize(event);

    if (lvglData->display == nullptr)
        return;

    lv_global = lvglData->global;
    lv_display_set_resolution(lvglData->display, event.size.getWidth(), event.size.getHeight());
    lv_refr_now(lvglData->display);
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

lv_global_t* lv_global_default()
{
	DISTRHO_SAFE_ASSERT(DGL_NAMESPACE::lv_global != nullptr);
    return DGL_NAMESPACE::lv_global;
}

// --------------------------------------------------------------------------------------------------------------------
