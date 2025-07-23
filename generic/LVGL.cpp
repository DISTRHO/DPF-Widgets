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

#if defined(DGL_CAIRO)
# include "Cairo.hpp"
#elif defined(DGL_OPENGL)
# include "OpenGL.hpp"
#endif

#include "../distrho/extra/RingBuffer.hpp"
#include "../distrho/extra/Sleep.hpp"
#include "../distrho/extra/Time.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

static thread_local lv_global_t* lv_global = nullptr;

template <class BaseWidget>
struct LVGLWidget<BaseWidget>::PrivateData {
    LVGLWidget<BaseWidget>* const self;
    lv_global_t* const global;

    lv_display_t* display = nullptr;
    lv_group_t* group = nullptr;
    lv_indev_t* keyboard = nullptr;
    lv_indev_t* mousepointer = nullptr;
    lv_indev_t* mousewheel = nullptr;

    bool mouseButtons[3] = {};
    lv_point_t mousePos = {};
    double mouseWheelDelta = 0.0;
    SmallStackRingBuffer keyBuffer;

   #if defined(DGL_CAIRO)
    cairo_surface_t* surface = nullptr;
   #elif defined(DGL_OPENGL)
    GLuint textureId = 0;
   #endif
    Size<uint> textureSize;
    uint8_t* textureData = nullptr;

    lv_area_t updatedArea = {};

    explicit PrivateData(LVGLWidget<BaseWidget>* const s)
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
        lv_delay_set_cb(msleep);
        lv_tick_set_cb(gettime_ms);

        const double scaleFactor = self->getTopLevelWidget()->getScaleFactor();
        const uint width = self->getWidth() ?: 640 * scaleFactor;
        const uint height = self->getHeight() ?: 480 * scaleFactor;

        lv_area_set(&updatedArea, 0, 0, width, height);

        display = lv_display_create(width, height);
        DISTRHO_SAFE_ASSERT_RETURN(display != nullptr,);

        lv_display_set_dpi(display, LV_DPI_DEF * scaleFactor);

        group = lv_group_create();
        lv_group_set_default(group);

        if (lv_indev_t* const indev = lv_indev_create())
        {
            keyboard = indev;
            lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
            lv_indev_set_read_cb(indev, indev_keyboard_read_cb);
            lv_indev_set_driver_data(indev, this);
            lv_indev_set_group(indev, group);
        }

        if (lv_indev_t* const indev = lv_indev_create())
        {
            mousepointer = indev;
            lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
            lv_indev_set_read_cb(indev, indev_mouse_read_cb);
            lv_indev_set_driver_data(indev, this);
            lv_indev_set_group(indev, group);
        }

        if (lv_indev_t* const indev = lv_indev_create())
        {
            mousewheel = indev;
            lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
            lv_indev_set_read_cb(indev, indev_mousewheel_read_cb);
            lv_indev_set_driver_data(indev, this);
            lv_indev_set_group(indev, group);
        }

       #ifdef DGL_OPENGL
        glGenTextures(1, &textureId);
        DISTRHO_SAFE_ASSERT_RETURN(textureId != 0,);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        static constexpr const float transparent[] = { 0.f, 0.f, 0.f, 0.f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, transparent);

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
       #endif

        lv_display_set_driver_data(display, this);
        lv_display_set_flush_cb(display, flush_cb);
        lv_display_add_event_cb(display, resolution_changed_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

        recreateTextureData(width, height);
    }

    void cleanup()
    {
        global->deinit_in_progress = true;

        if (keyboard != nullptr)
        {
            lv_indev_delete(keyboard);
            keyboard = nullptr;
        }

        if (mousepointer != nullptr)
        {
            lv_indev_delete(mousepointer);
            mousepointer = nullptr;
        }

        if (mousewheel != nullptr)
        {
            lv_indev_delete(mousewheel);
            mousewheel = nullptr;
        }

        if (group != nullptr)
        {
            lv_group_delete(group);
            group = nullptr;
        }

        if (display != nullptr)
        {
            lv_display_delete(display);
            display = nullptr;
        }

       #if defined(DGL_CAIRO)
        cairo_surface_destroy(surface);
        surface = nullptr;
       #elif defined(DGL_OPENGL)
        if (textureId != 0)
        {
            glDeleteTextures(1, &textureId);
            textureId = 0;
        }
       #endif

        std::free(textureData);
        textureData = nullptr;

        lv_deinit();
    }

    void recreateTextureData(const uint width, const uint height)
    {
        const lv_color_format_t lvformat = lv_display_get_color_format(display);
        const uint32_t stride = lv_draw_buf_width_to_stride(width, lvformat);
        const uint32_t data_size = stride * height;

        textureData = static_cast<uint8_t*>(std::realloc(textureData, data_size));
        std::memset(textureData, 0, data_size);

        textureSize = Size<uint>(width, height);
        lv_display_set_buffers(display, textureData, nullptr, data_size, LV_DISPLAY_RENDER_MODE_DIRECT);

       #ifdef DGL_CAIRO
        cairo_surface_destroy(surface);
        surface = cairo_image_surface_create_for_data(textureData, CAIRO_FORMAT_ARGB32, width, height, stride);
        DISTRHO_SAFE_ASSERT(surface != nullptr);
       #endif
    }

    void repaint(const Rectangle<uint>& rect);

    // ----------------------------------------------------------------------------------------------------------------

    static void msleep(const uint32_t millis) noexcept
    {
        return DISTRHO_NAMESPACE::d_msleep(millis);
    }

    static uint32_t gettime_ms() noexcept
    {
        return DISTRHO_NAMESPACE::d_gettime_ms();
    }

    // ----------------------------------------------------------------------------------------------------------------

    static void resolution_changed_cb(lv_event_t* const ev)
    {
        lv_display_t* const evdisplay = static_cast<lv_display_t*>(lv_event_get_current_target(ev));
        PrivateData* const evthis = static_cast<PrivateData*>(lv_display_get_driver_data(evdisplay));
        const uint width = lv_display_get_horizontal_resolution(evdisplay);
        const uint height = lv_display_get_vertical_resolution(evdisplay);

        evthis->recreateTextureData(width, height);
    }

    static void flush_cb(lv_display_t* const evdisplay, const lv_area_t* const area, uint8_t* const data)
    {
        PrivateData* const evthis = static_cast<PrivateData*>(lv_display_get_driver_data(evdisplay));

        if (evthis->updatedArea.x1 == 0 &&
            evthis->updatedArea.y1 == 0 &&
            evthis->updatedArea.x2 == 0 &&
            evthis->updatedArea.y2 == 0)
        {
            lv_area_copy(&evthis->updatedArea, area);
        }
        else
        {
            lv_area_t tmp;
            lv_area_copy(&tmp, &evthis->updatedArea);
            _lv_area_join(&evthis->updatedArea, &tmp, area);
        }

        evthis->repaint(Rectangle<uint>(evthis->updatedArea.x1,
                                        evthis->updatedArea.y1,
                                        evthis->updatedArea.x2 - evthis->updatedArea.x1,
                                        evthis->updatedArea.y2 - evthis->updatedArea.y1));

        lv_display_flush_ready(evdisplay);
    }

    // ----------------------------------------------------------------------------------------------------------------

    static void indev_keyboard_read_cb(lv_indev_t* const indev, lv_indev_data_t* const data)
    {
        PrivateData* const evthis = static_cast<PrivateData*>(lv_indev_get_driver_data(indev));

        if (evthis->keyBuffer.isDataAvailableForReading())
        {
            data->state = LV_INDEV_STATE_PRESSED;
            data->key = evthis->keyBuffer.readUShort();
            data->continue_reading = evthis->keyBuffer.isDataAvailableForReading();
        }
        else
        {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }

    static void indev_mouse_read_cb(lv_indev_t* const indev, lv_indev_data_t* const data)
    {
        PrivateData* const evthis = static_cast<PrivateData*>(lv_indev_get_driver_data(indev));

        data->point = evthis->mousePos;
        data->state = evthis->mouseButtons[kMouseButtonLeft] ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    }

    static void indev_mousewheel_read_cb(lv_indev_t* const indev, lv_indev_data_t* const data)
    {
        PrivateData* const evthis = static_cast<PrivateData*>(lv_indev_get_driver_data(indev));

        data->state = evthis->mouseButtons[kMouseButtonMiddle] ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        data->enc_diff = evthis->mouseWheelDelta;
        evthis->mouseWheelDelta = 0.0;
    }

    DISTRHO_DECLARE_NON_COPYABLE(PrivateData)
};

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
void LVGLWidget<BaseWidget>::idleCallback()
{
    lv_global = lvglData->global;
    lv_timer_handler();
}

template <class BaseWidget>
void LVGLWidget<BaseWidget>::onDisplay()
{
    DISTRHO_SAFE_ASSERT_RETURN(BaseWidget::getSize() == lvglData->textureSize,);

    const int32_t width = static_cast<int32_t>(BaseWidget::getWidth());
    const int32_t height = static_cast<int32_t>(BaseWidget::getHeight());

#if 0
    // TODO see what is really needed here..
    glColor4f(1.f, 1.f, 1.f, 1.f);
    // glClearColor();
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.f, 0.f);
        glVertex2d(0, 0);

        glTexCoord2f(1.f, 0.f);
        glVertex2d(width, 0);

        glTexCoord2f(1.f, 1.f);
        glVertex2d(width, height);

        glTexCoord2f(0.f, 1.f);
        glVertex2d(0, height);
    }
    glEnd();
#endif

   #if defined(DGL_CAIRO)
    if (lvglData->surface != nullptr)
    {
        cairo_t* const handle = static_cast<const CairoGraphicsContext&>(BaseWidget::getGraphicsContext()).handle;
        cairo_set_source_surface(handle, lvglData->surface, 0, 0);
        cairo_paint(handle);
    }

    lv_area_set(&lvglData->updatedArea, 0, 0, 0, 0);
   #elif defined(DGL_OPENGL)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, lvglData->textureId);

    if (lvglData->updatedArea.x1 != lvglData->updatedArea.x2 || lvglData->updatedArea.y1 != lvglData->updatedArea.y2)
    {
       #if LV_COLOR_DEPTH == 32
        static constexpr const GLenum format = GL_BGRA;
        static constexpr const GLenum ftype = GL_UNSIGNED_BYTE;
       #elif LV_COLOR_DEPTH == 24
        static constexpr const GLenum format = GL_BGR;
        static constexpr const GLenum ftype = GL_UNSIGNED_BYTE;
       #elif LV_COLOR_DEPTH == 16
        static constexpr const GLenum format = GL_RGB;
        static constexpr const GLenum ftype = GL_UNSIGNED_SHORT_5_6_5;
       #elif LV_COLOR_DEPTH == 8
        static constexpr const GLenum format = GL_LUMINANCE;
        static constexpr const GLenum ftype = GL_UNSIGNED_BYTE;
       #else
        #error Unsupported color format
       #endif

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

        // full size
        if (lvglData->updatedArea.x1 == 0 &&
            lvglData->updatedArea.y1 == 0 &&
            lvglData->updatedArea.x2 == width &&
            lvglData->updatedArea.y2 == height)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, ftype, lvglData->textureData);
        }
        // partial size
        else
        {
            const int32_t partial_x = lvglData->updatedArea.x1;
            const int32_t partial_y = lvglData->updatedArea.y1;
            const int32_t partial_width = lvglData->updatedArea.x2 - partial_x;
            const int32_t partial_height = lvglData->updatedArea.y2 - partial_y;

            const uint8_t colsize = lv_color_format_get_size(lv_display_get_color_format(lvglData->display));
            const int32_t offset = partial_y * width * colsize + partial_x * colsize;

            glTexSubImage2D(GL_TEXTURE_2D, 0,
                            partial_x,
                            partial_y,
                            partial_width,
                            partial_height,
                            format, ftype,
                            lvglData->textureData + offset);
        }

        lv_area_set(&lvglData->updatedArea, 0, 0, 0, 0);
    }

    glBegin(GL_QUADS);
    {
        glTexCoord2f(0.f, 0.f);
        glVertex2d(0, 0);

        glTexCoord2f(1.f, 0.f);
        glVertex2d(width, 0);

        glTexCoord2f(1.f, 1.f);
        glVertex2d(width, height);

        glTexCoord2f(0.f, 1.f);
        glVertex2d(0, height);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
   #endif
}

template <class BaseWidget>
bool LVGLWidget<BaseWidget>::onKeyboard(const Widget::KeyboardEvent& event)
{
    if (BaseWidget::onKeyboard(event))
        return true;

    if (! event.press)
        return false;

    uint16_t key;
    if (event.key >= kKeyF1)
    {
        switch (event.key)
        {
        case kKeyUp:
        case kKeyPadUp:
            key = LV_KEY_UP;
            break;
        case kKeyDown:
        case kKeyPadDown:
            key = LV_KEY_DOWN;
            break;
        case kKeyRight:
        case kKeyPadRight:
            key = LV_KEY_RIGHT;
            break;
        case kKeyLeft:
        case kKeyPadLeft:
            key = LV_KEY_LEFT;
            break;
        case kKeyHome:
        case kKeyPadHome:
            key = LV_KEY_HOME;
            break;
        case kKeyEnd:
        case kKeyPadEnd:
            key = LV_KEY_END;
            break;
        case kKeyPadEnter:
            key = LV_KEY_ENTER;
            break;
        case kKeyPad0: key = '0'; break;
        case kKeyPad1: key = '1'; break;
        case kKeyPad2: key = '2'; break;
        case kKeyPad3: key = '3'; break;
        case kKeyPad4: key = '4'; break;
        case kKeyPad5: key = '5'; break;
        case kKeyPad6: key = '6'; break;
        case kKeyPad7: key = '7'; break;
        case kKeyPad8: key = '8'; break;
        case kKeyPad9: key = '9'; break;
        case kKeyPadEqual: key = '='; break;
        case kKeyPadMultiply: key = '*'; break;
        case kKeyPadAdd: key = '+'; break;
        case kKeyPadSubtract: key = '-'; break;
        case kKeyPadDecimal: key = '.'; break;
        case kKeyPadDivide: key = '/'; break;
        default:
            return false;
        }
    }
    else
    {
        switch (event.key)
        {
        case kKeyEnter:
            key = LV_KEY_ENTER;
            break;
        case kKeyTab:
            key = event.mod & kModifierShift ? LV_KEY_PREV : LV_KEY_NEXT;
            break;
        default:
            key = event.key;
            break;
        }
    }

    lvglData->keyBuffer.writeUShort(key);
    lvglData->keyBuffer.commitWrite();
    return false;
}

template <class BaseWidget>
bool LVGLWidget<BaseWidget>::onMouse(const Widget::MouseEvent& event)
{
    if (BaseWidget::onMouse(event))
        return true;

    if (event.button > ARRAY_SIZE(lvglData->mouseButtons))
        return false;

    lvglData->mouseButtons[event.button] = event.press;
    return true;
}

template <>
bool LVGLWidget<SubWidget>::onMouse(const Widget::MouseEvent& event)
{
    if (SubWidget::onMouse(event))
        return true;

    if (!getAbsoluteArea().contains(event.absolutePos))
        return false;

    if (event.button > ARRAY_SIZE(lvglData->mouseButtons))
        return false;

    lvglData->mouseButtons[event.button] = event.press;
    return true;
}

template <class BaseWidget>
bool LVGLWidget<BaseWidget>::onMotion(const Widget::MotionEvent& event)
{
    if (BaseWidget::onMotion(event))
        return true;

    lvglData->mousePos.x = std::max(0, std::min<int>(BaseWidget::getWidth() - 1, event.pos.getX()));
    lvglData->mousePos.y = std::max(0, std::min<int>(BaseWidget::getHeight() - 1, event.pos.getY()));
    return true;
}

template <>
bool LVGLWidget<SubWidget>::onMotion(const Widget::MotionEvent& event)
{
    if (SubWidget::onMotion(event))
        return true;

    if (!getAbsoluteArea().contains(event.absolutePos))
        return false;

    lvglData->mousePos.x = std::max(0, std::min<int>(getWidth() - 1, event.pos.getX()));
    lvglData->mousePos.y = std::max(0, std::min<int>(getHeight() - 1, event.pos.getY()));
    return true;
}

template <class BaseWidget>
bool LVGLWidget<BaseWidget>::onScroll(const Widget::ScrollEvent& event)
{
    if (BaseWidget::onScroll(event))
        return true;

    lvglData->mouseWheelDelta -= event.delta.getY();
    return false;
}

template <>
bool LVGLWidget<SubWidget>::onScroll(const Widget::ScrollEvent& event)
{
    if (SubWidget::onScroll(event))
        return true;

    if (!getAbsoluteArea().contains(event.absolutePos))
        return false;

    lvglData->mouseWheelDelta -= event.delta.getY();
    return false;
}

template <class BaseWidget>
void LVGLWidget<BaseWidget>::onResize(const Widget::ResizeEvent& event)
{
    BaseWidget::onResize(event);

    if (lvglData->display == nullptr)
        return;

    const uint width = event.size.getWidth();
    const uint height = event.size.getHeight();
    lv_area_set(&lvglData->updatedArea, 0, 0, width, height);

    lv_global = lvglData->global;
    lv_display_set_resolution(lvglData->display, width, height);
    lv_refr_now(lvglData->display);
}

// --------------------------------------------------------------------------------------------------------------------
// LVGLSubWidget

template <>
LVGLWidget<SubWidget>::LVGLWidget(Widget* const parent)
    : SubWidget(parent),
      lvglData(new PrivateData(this))
{
    getWindow().addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
LVGLWidget<SubWidget>::~LVGLWidget()
{
    getWindow().removeIdleCallback(this);
    delete lvglData;
}

template <>
void LVGLWidget<SubWidget>::PrivateData::repaint(const Rectangle<uint>&)
{
    self->repaint();
}

template class LVGLWidget<SubWidget>;

// --------------------------------------------------------------------------------------------------------------------
// LVGLTopLevelWidget

template <>
LVGLWidget<TopLevelWidget>::LVGLWidget(Window& windowToMapTo)
    : TopLevelWidget(windowToMapTo),
      lvglData(new PrivateData(this))
{
    addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
LVGLWidget<TopLevelWidget>::~LVGLWidget()
{
    removeIdleCallback(this);
    delete lvglData;
}

template <>
void LVGLWidget<TopLevelWidget>::PrivateData::repaint(const Rectangle<uint>& rect)
{
    self->repaint(rect);
}

template class LVGLWidget<TopLevelWidget>;

// --------------------------------------------------------------------------------------------------------------------
// LVGLStandaloneWindow

template <>
LVGLWidget<StandaloneWindow>::LVGLWidget(Application& app)
    : StandaloneWindow(app),
      lvglData(new PrivateData(this))
{
    Window::addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
LVGLWidget<StandaloneWindow>::LVGLWidget(Application& app, Window& transientParentWindow)
    : StandaloneWindow(app, transientParentWindow),
      lvglData(new PrivateData(this))
{
    Window::addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
LVGLWidget<StandaloneWindow>::~LVGLWidget()
{
    Window::removeIdleCallback(this);
    delete lvglData;
}

template <>
void LVGLWidget<StandaloneWindow>::PrivateData::repaint(const Rectangle<uint>& rect)
{
    self->repaint(rect);
}

template class LVGLWidget<StandaloneWindow>;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

lv_global_t* lv_global_default()
{
	DISTRHO_SAFE_ASSERT(DGL_NAMESPACE::lv_global != nullptr);
    return DGL_NAMESPACE::lv_global;
}

// --------------------------------------------------------------------------------------------------------------------
