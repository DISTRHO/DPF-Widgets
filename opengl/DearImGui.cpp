/*
 * Dear ImGui for DPF
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
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

#ifndef IMGUI_SKIP_IMPLEMENTATION
# define IMGUI_DPF_BACKEND
# include "DearImGui/imgui.cpp"
# include "DearImGui/imgui_demo.cpp"
# include "DearImGui/imgui_draw.cpp"
# include "DearImGui/imgui_tables.cpp"
# include "DearImGui/imgui_widgets.cpp"
# ifdef DGL_USE_OPENGL3
#  include "DearImGui/imgui_impl_opengl3.cpp"
# else
#  include "DearImGui/imgui_impl_opengl2.cpp"
# endif
#else
# ifdef DGL_USE_OPENGL3
#  include "DearImGui/imgui_impl_opengl3.h"
# else
#  include "DearImGui/imgui_impl_opengl2.h"
# endif
#endif

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

int mouseButtonToImGui(int button)
{
    switch (button)
    {
    case 1:
        return 0;
    case 2:
        return 2;
    case 3:
        return 1;
    default:
        return -1;
    }
}

template <class BaseWidget>
struct ImGuiWidget<BaseWidget>::PrivateData {
    ImGuiWidget<BaseWidget>* const self;
    ImGuiContext* context;

    explicit PrivateData(ImGuiWidget<BaseWidget>* const s)
        : self(s),
          context(nullptr)
    {
        IMGUI_CHECKVERSION();
        context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);

        const double scaleFactor = self->getTopLevelWidget()->getScaleFactor();

        ImGuiIO& io(ImGui::GetIO());
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.DisplaySize.x = std::round(scaleFactor * self->getWidth());
        io.DisplaySize.y = std::round(scaleFactor * self->getHeight());
        io.IniFilename = nullptr;

        /*
        io.KeyMap[ImGuiKey_Tab] = '\t';
        io.KeyMap[ImGuiKey_LeftArrow] = IM_ARRAYSIZE(io.KeysDown) - kKeyLeft;
        io.KeyMap[ImGuiKey_RightArrow] = IM_ARRAYSIZE(io.KeysDown) - kKeyRight;
        io.KeyMap[ImGuiKey_UpArrow] = IM_ARRAYSIZE(io.KeysDown) - kKeyUp;
        io.KeyMap[ImGuiKey_DownArrow] = IM_ARRAYSIZE(io.KeysDown) - kKeyDown;
        io.KeyMap[ImGuiKey_PageUp] = IM_ARRAYSIZE(io.KeysDown) - kKeyPageUp;
        io.KeyMap[ImGuiKey_PageDown] = IM_ARRAYSIZE(io.KeysDown) - kKeyPageDown;
        io.KeyMap[ImGuiKey_Home] = IM_ARRAYSIZE(io.KeysDown) - kKeyHome;
        io.KeyMap[ImGuiKey_End] = IM_ARRAYSIZE(io.KeysDown) - kKeyEnd;
        io.KeyMap[ImGuiKey_Insert] = IM_ARRAYSIZE(io.KeysDown) - kKeyInsert;
        io.KeyMap[ImGuiKey_Delete] = 127;
        io.KeyMap[ImGuiKey_Backspace] = '\b';
        io.KeyMap[ImGuiKey_Space] = ' ';
        io.KeyMap[ImGuiKey_Enter] = '\r';
        io.KeyMap[ImGuiKey_Escape] = 27;
        io.KeyMap[ImGuiKey_A] = 'A';
        io.KeyMap[ImGuiKey_C] = 'C';
        io.KeyMap[ImGuiKey_V] = 'V';
        io.KeyMap[ImGuiKey_X] = 'X';
        io.KeyMap[ImGuiKey_Y] = 'Y';
        io.KeyMap[ImGuiKey_Z] = 'Z';
        */

#ifdef DGL_USE_OPENGL3
        ImGui_ImplOpenGL3_Init();
#else
        ImGui_ImplOpenGL2_Init();
#endif
    }

    ~PrivateData()
    {
        ImGui::SetCurrentContext(context);
#ifdef DGL_USE_OPENGL3
        ImGui_ImplOpenGL3_Shutdown();
#else
        ImGui_ImplOpenGL2_Shutdown();
#endif
        ImGui::DestroyContext(context);
    }

    DISTRHO_DECLARE_NON_COPYABLE(PrivateData)
};

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
void ImGuiWidget<BaseWidget>::idleCallback()
{
    /*
    bool shouldRepaint;

    if (fImpl->fWasEverPainted)
    {
        Impl::Clock::duration elapsed =
            Impl::Clock::now() - fImpl->fLastRepainted;
        std::chrono::milliseconds elapsedMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
        shouldRepaint = elapsedMs.count() > fImpl->fRepaintIntervalMs;
    }
    else
    {
        shouldRepaint = true;
    }

    if (shouldRepaint)
        repaint();
    */
    BaseWidget::repaint();
}

template <>
void ImGuiWidget<SubWidget>::onDisplay()
{
    ImGui::SetCurrentContext(pData->context);

#ifdef DGL_USE_OPENGL3
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
        data->DisplayPos.x = -getAbsoluteX();
        data->DisplayPos.y = getWindow().getHeight() - (getAbsoluteY() + getHeight());
#ifdef DGL_USE_OPENGL3
        ImGui_ImplOpenGL3_RenderDrawData(data);
#else
        ImGui_ImplOpenGL2_RenderDrawData(data);
#endif
    }

    /*
    fImpl->fLastRepainted = Impl::Clock::now();
    fImpl->fWasEverPainted = true;
    */
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onKeyboard(const Widget::KeyboardEvent& event)
{
    /*
    ImGui::SetCurrentContext(pData->context);

    ImGuiIO& io(ImGui::GetIO());

    if (event.press)
        io.AddInputCharacter(event.key);

    int imGuiKey = event.key;
    if (imGuiKey >= 0 && imGuiKey < 128)
    {
        if (imGuiKey >= 'a' && imGuiKey <= 'z')
            imGuiKey = imGuiKey - 'a' + 'A';
        io.KeysDown[imGuiKey] = event.press;
    }

    return io.WantCaptureKeyboard;
    */

    return BaseWidget::onKeyboard(event);
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onSpecial(const Widget::SpecialEvent& event)
{
    /*
    ImGui::SetCurrentContext(pData->context);

    ImGuiIO& io(ImGui::GetIO());

    int imGuiKey = IM_ARRAYSIZE(io.KeysDown) - event.key;
    io.KeysDown[imGuiKey] = event.press;

    switch (event.key)
    {
    case kKeyShift:
        io.KeyShift = event.press;
        break;
    case kKeyControl:
        io.KeyCtrl = event.press;
        break;
    case kKeyAlt:
        io.KeyAlt = event.press;
        break;
    case kKeySuper:
        io.KeySuper = event.press;
        break;
    default:
        break;
    }

    return io.WantCaptureKeyboard;
    */

    return BaseWidget::onSpecial(event);
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onMouse(const Widget::MouseEvent& event)
{
    ImGui::SetCurrentContext(pData->context);

    ImGuiIO& io(ImGui::GetIO());

    int imGuiButton = mouseButtonToImGui(event.button);
    if (imGuiButton != -1)
        io.MouseDown[imGuiButton] = event.press;

    return io.WantCaptureMouse;

    return BaseWidget::onMouse(event);
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onMotion(const Widget::MotionEvent& event)
{
    ImGui::SetCurrentContext(pData->context);

    ImGuiIO& io(ImGui::GetIO());

    // FIXME
    const double scaleFactor = 1; // getScaleFactor();
    io.MousePos.x = std::round(scaleFactor * event.pos.getX());
    io.MousePos.y = std::round(scaleFactor * event.pos.getY());

    return BaseWidget::onMotion(event);
}

template <class BaseWidget>
bool ImGuiWidget<BaseWidget>::onScroll(const Widget::ScrollEvent& event)
{
    ImGui::SetCurrentContext(pData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.MouseWheel += event.delta.getY();
    io.MouseWheelH += event.delta.getX();

    return io.WantCaptureMouse;

    return BaseWidget::onScroll(event);
}

template <class BaseWidget>
void ImGuiWidget<BaseWidget>::onResize(const Widget::ResizeEvent& event)
{
    BaseWidget::onResize(event);

    const double scaleFactor = BaseWidget::getTopLevelWidget()->getScaleFactor();

    const uint width = event.size.getWidth();
    const uint height = event.size.getHeight();

    ImGui::SetCurrentContext(pData->context);

    ImGuiIO& io(ImGui::GetIO());
    io.DisplaySize.x = std::round(scaleFactor * width);
    io.DisplaySize.y = std::round(scaleFactor * height);
}

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
ImGuiWidget<BaseWidget>::~ImGuiWidget()
{
    delete pData;
}

// --------------------------------------------------------------------------------------------------------------------
// ImGuiSubWidget

template <>
ImGuiWidget<SubWidget>::ImGuiWidget(Widget* const parent)
    : SubWidget(parent),
      pData(new PrivateData(this))
{
    getTopLevelWidget()->addIdleCallback(this);
}

template class ImGuiWidget<SubWidget>;

// --------------------------------------------------------------------------------------------------------------------
// ImGuiTopLevelWidget

template <>
ImGuiWidget<TopLevelWidget>::ImGuiWidget(Window& windowToMapTo)
    : TopLevelWidget(windowToMapTo),
      pData(new PrivateData(this))
{
    TopLevelWidget::addIdleCallback(this);
}

template class ImGuiWidget<TopLevelWidget>;

// --------------------------------------------------------------------------------------------------------------------
// ImGuiStandaloneWindow

template <>
ImGuiWidget<StandaloneWindow>::ImGuiWidget(Application& app)
    : StandaloneWindow(app),
      pData(new PrivateData(this))
{
    Window::addIdleCallback(this);
}

template <>
ImGuiWidget<StandaloneWindow>::ImGuiWidget(Application& app, Window& transientParentWindow)
    : StandaloneWindow(app, transientParentWindow),
      pData(new PrivateData(this))
{
    Window::addIdleCallback(this);
}

template class ImGuiWidget<StandaloneWindow>;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
