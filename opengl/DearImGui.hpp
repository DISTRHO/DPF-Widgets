/*
 * Dear ImGui for DPF
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
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

#pragma once

#include "SubWidget.hpp"
#include "TopLevelWidget.hpp"
#include "StandaloneWindow.hpp"

#include "DearImGui/imgui.h"
#include "DearImGuiKnobs/imgui-knobs.h"
#include "DearImGuiToggle/imgui_toggle.h"
#include "DearImGuiToggle/imgui_toggle_math.h"
#include "DearImGuiToggle/imgui_toggle_palette.h"
#include "DearImGuiToggle/imgui_toggle_presets.h"
#include "DearImGuiToggle/imgui_toggle_renderer.h"

#ifdef DISTRHO_UI_HPP_INCLUDED
START_NAMESPACE_DISTRHO
class UI;
class UIExporter;
END_NAMESPACE_DISTRHO
#endif

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

/**
   Dear ImGui Widget class.

   This class exposes the Dear ImGui drawing API inside a DGL Widget.
   The drawing function onDisplay() is implemented internally
   but a new onImGuiDisplay() needs to be overridden instead.

   This class will take care of setting up ImGui for drawing,
   and also also user input, resizes and everything in between.
 */
template <class BaseWidget>
class ImGuiWidget : public BaseWidget,
                    public IdleCallback
{
public:
   /**
      Constructor for a ImGuiSubWidget.
    */
    explicit ImGuiWidget(Widget* parentGroupWidget, float fontSize = 13.f);

   /**
      Constructor for a ImGuiTopLevelWidget.
    */
    explicit ImGuiWidget(Window& windowToMapTo, float fontSize = 13.f);

   /**
      Constructor for a ImGuiStandaloneWindow without transient parent window.
    */
    explicit ImGuiWidget(Application& app, float fontSize = 13.f);

   /**
      Constructor for a ImGuiStandaloneWindow with transient parent window.
    */
    explicit ImGuiWidget(Application& app, Window& transientParentWindow, float fontSize = 13.f);

   /**
      Destructor.
    */
    ~ImGuiWidget() override;

   /**
      Change global font size.
    */
    void setFontSize(float fontSize);

protected:
   /**
      New virtual onDisplay function.
      @see onDisplay
    */
    virtual void onImGuiDisplay() = 0;

    void idleCallback() override;
    void onDisplay() override;
    bool onKeyboard(const Widget::KeyboardEvent& event) override;
    bool onCharacterInput(const Widget::CharacterInputEvent&) override;
    bool onMouse(const Widget::MouseEvent& event) override;
    bool onMotion(const Widget::MotionEvent& event) override;
    bool onScroll(const Widget::ScrollEvent& event) override;
    void onResize(const Widget::ResizeEvent& event) override;

    struct PrivateData;
    PrivateData* const imData;

#ifdef DISTRHO_UI_HPP_INCLUDED
    friend class DISTRHO_NAMESPACE::UI;
    friend class DISTRHO_NAMESPACE::UIExporter;
#endif

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiWidget)
};

typedef ImGuiWidget<SubWidget> ImGuiSubWidget;
typedef ImGuiWidget<TopLevelWidget> ImGuiTopLevelWidget;
typedef ImGuiWidget<StandaloneWindow> ImGuiStandaloneWindow;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
