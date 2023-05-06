/*
 * Dear ImGui for DPF
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
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

#include "DearImGui.hpp"
#include "Application.hpp"

#ifndef DGL_NO_SHARED_RESOURCES
# include "src/Resources.hpp"
#endif

#if defined(DGL_USE_GLES2)
# define IMGUI_IMPL_OPENGL_ES2
#elif defined(DGL_USE_GLES3)
# define IMGUI_IMPL_OPENGL_ES3
#elif defined(DGL_USE_OPENGL3)
# define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#ifndef IMGUI_SKIP_IMPLEMENTATION
# define IMGUI_DPF_BACKEND
# include "DearImGui/imgui.cpp"
# include "DearImGui/imgui_demo.cpp"
# include "DearImGui/imgui_draw.cpp"
# include "DearImGui/imgui_tables.cpp"
# include "DearImGui/imgui_widgets.cpp"
# include "DearImGuiKnobs/imgui-knobs.cpp"
# include "DearImGuiToggle/imgui_toggle.cpp"
# include "DearImGuiToggle/imgui_toggle_palette.cpp"
# include "DearImGuiToggle/imgui_toggle_presets.cpp"
# include "DearImGuiToggle/imgui_toggle_renderer.cpp"

# if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
#  include "DearImGui/imgui_impl_opengl3.cpp"
# else
#  include "DearImGui/imgui_impl_opengl2.cpp"
# endif
#else
# if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
#  include "DearImGui/imgui_impl_opengl3.h"
# else
#  include "DearImGui/imgui_impl_opengl2.h"
# endif
#endif

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

static const char* GetClipboardTextFn(void* const userData)
{
    TopLevelWidget* const tlw = static_cast<TopLevelWidget*>(userData);

    size_t dataSize;
    return static_cast<const char*>(tlw->getClipboard(dataSize));
}

static void SetClipboardTextFn(void* const userData, const char* const text)
{
    TopLevelWidget* const tlw = static_cast<TopLevelWidget*>(userData);
    DISTRHO_SAFE_ASSERT_RETURN(tlw!= nullptr,);
    DISTRHO_SAFE_ASSERT_RETURN(text != nullptr,);

    tlw->setClipboard(nullptr, text, std::strlen(text)+1);
}

template <class BaseWidget>
struct ImGuiWidget<BaseWidget>::PrivateData {
    ImGuiWidget<BaseWidget>* const self;
    ImGuiContext* context;
    double scaleFactor;
    double lastFrameTime;

    explicit PrivateData(ImGuiWidget<BaseWidget>* const s, const float fontSize)
        : self(s),
          context(nullptr),
          scaleFactor(s->getTopLevelWidget()->getScaleFactor()),
          lastFrameTime(0.0)
    {
        IMGUI_CHECKVERSION();
        context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);

        ImGuiIO& io(ImGui::GetIO());
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.DisplaySize.x = self->getWidth();
        io.DisplaySize.y = self->getHeight();
        // not needed, we handle this ourselves
        // io.DisplayFramebufferScale = ImVec2(scaleFactor, scaleFactor);
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;

        ImGuiStyle& style(ImGui::GetStyle());
        style.ScaleAllSizes(scaleFactor);

       #ifndef DGL_NO_SHARED_RESOURCES
        using namespace dpf_resources;
        ImFontConfig fc;
        fc.FontDataOwnedByAtlas = false;
        fc.OversampleH = 1;
        fc.OversampleV = 1;
        fc.PixelSnapH = true;
        io.Fonts->AddFontFromMemoryTTF((void*)dejavusans_ttf, dejavusans_ttf_size, fontSize * scaleFactor, &fc);
        io.Fonts->Build();
       #endif

        io.KeyMap[ImGuiKey_Tab] = '\t';
        io.KeyMap[ImGuiKey_LeftArrow] = 0xff + kKeyLeft - kKeyF1;
        io.KeyMap[ImGuiKey_RightArrow] = 0xff + kKeyRight - kKeyF1;
        io.KeyMap[ImGuiKey_UpArrow] = 0xff + kKeyUp - kKeyF1;
        io.KeyMap[ImGuiKey_DownArrow] = 0xff + kKeyDown - kKeyF1;
        io.KeyMap[ImGuiKey_PageUp] = 0xff + kKeyPageUp - kKeyF1;
        io.KeyMap[ImGuiKey_PageDown] = 0xff + kKeyPageDown - kKeyF1;
        io.KeyMap[ImGuiKey_Home] = 0xff + kKeyHome - kKeyF1;
        io.KeyMap[ImGuiKey_End] = 0xff + kKeyEnd - kKeyF1;
        io.KeyMap[ImGuiKey_Insert] = 0xff + kKeyInsert - kKeyF1;
        io.KeyMap[ImGuiKey_Delete] = kKeyDelete;
        io.KeyMap[ImGuiKey_Backspace] = kKeyBackspace;
        io.KeyMap[ImGuiKey_Space] = ' ';
        io.KeyMap[ImGuiKey_Enter] = '\r';
        io.KeyMap[ImGuiKey_Escape] = kKeyEscape;
        // io.KeyMap[ImGuiKey_KeyPadEnter] = '\n';
        io.KeyMap[ImGuiKey_A] = 'a';
        io.KeyMap[ImGuiKey_C] = 'c';
        io.KeyMap[ImGuiKey_V] = 'v';
        io.KeyMap[ImGuiKey_X] = 'x';
        io.KeyMap[ImGuiKey_Y] = 'y';
        io.KeyMap[ImGuiKey_Z] = 'z';

        io.GetClipboardTextFn = GetClipboardTextFn;
        io.SetClipboardTextFn = SetClipboardTextFn;
        io.ClipboardUserData = s->getTopLevelWidget();

       #if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
        ImGui_ImplOpenGL3_Init();
       #else
        ImGui_ImplOpenGL2_Init();
       #endif
    }

    ~PrivateData()
    {
        ImGui::SetCurrentContext(context);
       #if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
        ImGui_ImplOpenGL3_Shutdown();
       #else
        ImGui_ImplOpenGL2_Shutdown();
       #endif
        ImGui::DestroyContext(context);
    }

    float getDisplayX() const noexcept;
    float getDisplayY() const noexcept;
    double getTime() const noexcept;

    inline double getTimeDelta() noexcept
    {
        const double time = getTime();
        const double delta = time - lastFrameTime;
        lastFrameTime = time;
        return delta;
    }

    DISTRHO_DECLARE_NON_COPYABLE(PrivateData)
};

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
void ImGuiWidget<BaseWidget>::setFontSize(const float fontSize)
{
    ImGui::SetCurrentContext(imData->context);
    ImGuiIO& io(ImGui::GetIO());

    const double scaleFactor = BaseWidget::getTopLevelWidget()->getScaleFactor();

   #ifndef DGL_NO_SHARED_RESOURCES
    using namespace dpf_resources;
    ImFontConfig fc;
    fc.FontDataOwnedByAtlas = false;
    fc.OversampleH = 1;
    fc.OversampleV = 1;
    fc.PixelSnapH = true;
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryTTF((void*)dejavusans_ttf, dejavusans_ttf_size, fontSize * scaleFactor, &fc);
    io.Fonts->Build();
   #endif
}

template <class BaseWidget>
void ImGuiWidget<BaseWidget>::idleCallback()
{
    BaseWidget::repaint();
}

template <class BaseWidget>
void ImGuiWidget<BaseWidget>::onDisplay()
{
    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    io.DeltaTime = imData->getTimeDelta();

   #if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
    ImGui_ImplOpenGL3_NewFrame();
   #else
    ImGui_ImplOpenGL2_NewFrame();
   #endif

    ImGui::NewFrame();
    onImGuiDisplay();
    ImGui::Render();

    /*
    const GraphicsContext& gc(getGraphicsContext());
    Color::fromHTML("#373").setFor(gc);
    Rectangle<double> r(0, 0, getWidth(), getHeight());
    r.draw(gc);
    */

    if (ImDrawData* const data = ImGui::GetDrawData())
    {
        data->DisplayPos.x = -imData->getDisplayX();
        data->DisplayPos.y = imData->getDisplayY();
       #if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
        ImGui_ImplOpenGL3_RenderDrawData(data);
       #else
        ImGui_ImplOpenGL2_RenderDrawData(data);
       #endif
    }
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onKeyboard(const Widget::KeyboardEvent& event)
{
    if (BaseWidget::onKeyboard(event))
        return true;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    io.KeyCtrl  = event.mod & kModifierControl;
    io.KeyShift = event.mod & kModifierShift;
    io.KeyAlt   = event.mod & kModifierAlt;
    io.KeySuper = event.mod & kModifierSuper;

    // d_stdout("onKeyboard %u %u", event.key, event.keycode);

    if (event.key <= kKeyDelete)
        io.KeysDown[event.key] = event.press;
    else if (event.key >= kKeyF1 && event.key <= kKeyPause)
        io.KeysDown[0xff + event.key - kKeyF1] = event.press;

    return io.WantCaptureKeyboard;
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onCharacterInput(const Widget::CharacterInputEvent& event)
{
    if (BaseWidget::onCharacterInput(event))
        return true;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    switch (event.character)
    {
    case kKeyBackspace:
    case kKeyEscape:
    case kKeyDelete:
    case '\n':
    case '\r':
    case '\t':
        break;
    default:
        // d_stdout("input %u %u %lu '%s'", event.keycode, event.character, std::strlen(event.string), event.string);
        io.AddInputCharactersUTF8(event.string);
        break;
    }

    return io.WantCaptureKeyboard;
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onMouse(const Widget::MouseEvent& event)
{
    if (BaseWidget::onMouse(event))
        return true;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());

    switch (event.button)
    {
    case kMouseButtonLeft:
        io.MouseDown[0] = event.press;
        break;
    case kMouseButtonRight:
        io.MouseDown[1] = event.press;
        break;
    case kMouseButtonMiddle:
        io.MouseDown[2] = event.press;
        break;
    }

    return io.WantCaptureMouse;
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onMotion(const Widget::MotionEvent& event)
{
    if (BaseWidget::onMotion(event))
        return true;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MousePos.x = event.pos.getX();
    io.MousePos.y = event.pos.getY();
    return false;
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onScroll(const Widget::ScrollEvent& event)
{
    if (BaseWidget::onScroll(event))
        return true;

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MouseWheel += event.delta.getY();
    io.MouseWheelH += event.delta.getX();
    return io.WantCaptureMouse;
}

template <class BaseWidget>
void ImGuiWidget<BaseWidget>::onResize(const Widget::ResizeEvent& event)
{
    BaseWidget::onResize(event);

    ImGui::SetCurrentContext(imData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.DisplaySize.x = event.size.getWidth();
    io.DisplaySize.y = event.size.getHeight();
}

// --------------------------------------------------------------------------------------------------------------------
// ImGuiSubWidget

template <>
float ImGuiWidget<SubWidget>::PrivateData::getDisplayX() const noexcept
{
    return self->getAbsoluteX();
}

template <>
float ImGuiWidget<SubWidget>::PrivateData::getDisplayY() const noexcept
{
    return self->getWindow().getHeight() - self->getAbsoluteY() - self->getHeight();
}

template <>
double ImGuiWidget<SubWidget>::PrivateData::getTime() const noexcept
{
    return self->getApp().getTime();
}

template <>
ImGuiWidget<SubWidget>::ImGuiWidget(Widget* const parent, const float fontSize)
    : SubWidget(parent),
      imData(new PrivateData(this, fontSize))
{
    getWindow().addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
ImGuiWidget<SubWidget>::~ImGuiWidget()
{
    getWindow().removeIdleCallback(this);
    delete imData;
}

template class ImGuiWidget<SubWidget>;

// --------------------------------------------------------------------------------------------------------------------
// ImGuiTopLevelWidget

template <>
float ImGuiWidget<TopLevelWidget>::PrivateData::getDisplayX() const noexcept
{
    return 0.0f;
}

template <>
float ImGuiWidget<TopLevelWidget>::PrivateData::getDisplayY() const noexcept
{
    return 0.0f;
}

template <>
double ImGuiWidget<TopLevelWidget>::PrivateData::getTime() const noexcept
{
    return self->getApp().getTime();
}

template <>
ImGuiWidget<TopLevelWidget>::ImGuiWidget(Window& windowToMapTo, const float fontSize)
    : TopLevelWidget(windowToMapTo),
      imData(new PrivateData(this, fontSize))
{
    addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
ImGuiWidget<TopLevelWidget>::~ImGuiWidget()
{
    removeIdleCallback(this);
    delete imData;
}

template class ImGuiWidget<TopLevelWidget>;

// --------------------------------------------------------------------------------------------------------------------
// ImGuiStandaloneWindow

template <>
float ImGuiWidget<StandaloneWindow>::PrivateData::getDisplayX() const noexcept
{
    return 0.0f;
}

template <>
float ImGuiWidget<StandaloneWindow>::PrivateData::getDisplayY() const noexcept
{
    return 0.0f;
}

template <>
double ImGuiWidget<StandaloneWindow>::PrivateData::getTime() const noexcept
{
    return self->getApp().getTime();
}

template <>
ImGuiWidget<StandaloneWindow>::ImGuiWidget(Application& app, const float fontSize)
    : StandaloneWindow(app),
      imData(new PrivateData(this, fontSize))
{
    Window::addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
ImGuiWidget<StandaloneWindow>::ImGuiWidget(Application& app, Window& transientParentWindow, const float fontSize)
    : StandaloneWindow(app, transientParentWindow),
      imData(new PrivateData(this, fontSize))
{
    Window::addIdleCallback(this, 1000 / 60); // 60 fps
}

template <>
ImGuiWidget<StandaloneWindow>::~ImGuiWidget()
{
    Window::removeIdleCallback(this);
    delete imData;
}

template class ImGuiWidget<StandaloneWindow>;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
