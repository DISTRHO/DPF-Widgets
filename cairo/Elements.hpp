/*
 * Cycfi Elements for DPF
 * Copyright (C) 2022 Filipe Coelho <falktx@falktx.com>
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

// TODO patch include paths for Elements/include/ prefix
#include "Elements/elements.hpp"

#if defined(ELEMENTS_HOST_UI_LIBRARY_COCOA) || defined(ELEMENTS_HOST_UI_LIBRARY_GTK) || defined(ELEMENTS_HOST_UI_LIBRARY_WIN32)
# error invalid use of Elements with DPF, Elements host UI must be platform agnostic
#endif

#include "Cairo.hpp"
#include "SubWidget.hpp"
#include "TopLevelWidget.hpp"
#include "StandaloneWindow.hpp"

#ifdef DISTRHO_UI_HPP_INCLUDED
START_NAMESPACE_DISTRHO
class UI;
class UIExporter;
END_NAMESPACE_DISTRHO
#endif

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
class ElementsWidget : public BaseWidget,
                       public cycfi::elements::view,
                       public IdleCallback
{
public:
   /**
      Constructor for a ImGuiStandaloneWindow without transient parent window.
    */
    explicit ElementsWidget(Application& app);

   /**
      Destructor.
    */
    ~ElementsWidget() override;

private:
    void onDisplay() override;
    void idleCallback() override;

   #ifdef DISTRHO_UI_HPP_INCLUDED
    friend class DISTRHO_NAMESPACE::UI;
    friend class DISTRHO_NAMESPACE::UIExporter;
   #endif

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElementsWidget)
};

// typedef ElementsWidget<SubWidget> ElementsSubWidget;
// typedef ElementsWidget<TopLevelWidget> ElementsTopLevelWidget;
typedef ElementsWidget<StandaloneWindow> ElementsStandaloneWindow;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
