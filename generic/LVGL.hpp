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

#pragma once

#if __cplusplus < 201103L
# error DPF LVGL widgets require C++11
#endif

#include "SubWidget.hpp"
#include "TopLevelWidget.hpp"
#include "StandaloneWindow.hpp"

#if !(defined(DGL_CAIRO) || defined(DGL_OPENGL))
# error LVGL must use Cairo or OpenGL
#endif

#include "lvgl.h"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

#if !defined(LV_ENABLE_GLOBAL_CUSTOM) || LV_ENABLE_GLOBAL_CUSTOM != 1
# error LV_ENABLE_GLOBAL_CUSTOM must be set to 1 for DPF builds
#endif

#if defined(DGL_CAIRO) && LV_COLOR_DEPTH != 32
# error LV_COLOR_DEPTH must be 32 for Cairo DPF builds
#endif

#if LV_DEF_REFR_PERIOD != 1
# error LV_DEF_REFR_PERIOD must be 1 for DPF builds
#endif

#if LV_DPI_DEF != 160
# error LV_DPI_DEF must be 160 for DPF builds
#endif

#if LV_USE_STDLIB_MALLOC != LV_STDLIB_CLIB
# error LV_USE_STDLIB_MALLOC must be LV_STDLIB_CLIB for DPF builds
#endif

#if LV_USE_STDLIB_STRING != LV_STDLIB_CLIB
# error LV_USE_STDLIB_STRING must be LV_STDLIB_CLIB for DPF builds
#endif

#if LV_USE_STDLIB_SPRINTF != LV_STDLIB_CLIB
# error LV_USE_STDLIB_SPRINTF must be LV_STDLIB_CLIB for DPF builds
#endif

#if defined(LV_USE_OS) && LV_USE_OS != LV_OS_NONE
# error LV_USE_OS must be LV_OS_NONE for DPF builds
#endif

#if defined(LV_USE_EVDEV) && LV_USE_EVDEV
# error LV_USE_EVDEV must not be enabled for DPF builds
#endif

#if defined(LV_USE_LIBINPUT) && LV_USE_LIBINPUT
# error LV_USE_LIBINPUT must not be enabled for DPF builds
#endif

#if defined(LV_USE_LINUX_DRM) && LV_USE_LINUX_DRM
# error LV_USE_LINUX_DRM must not be enabled for DPF builds
#endif

#if defined(LV_USE_LINUX_FBDEV) && LV_USE_LINUX_FBDEV
# error LV_USE_LINUX_FBDEV must not be enabled for DPF builds
#endif

#if defined(LV_USE_NUTTX) && LV_USE_NUTTX
# error LV_USE_NUTTX must not be enabled for DPF builds
#endif

#if defined(LV_USE_SDL) && LV_USE_SDL
# error LV_USE_SDL must not be enabled for DPF builds
#endif

#if defined(LV_USE_WINDOWS) && LV_USE_WINDOWS
# error LV_USE_WINDOWS must not be enabled for DPF builds
#endif

#if defined(LV_USE_X11) && LV_USE_X11
# error LV_USE_X11 must not be enabled for DPF builds
#endif

// --------------------------------------------------------------------------------------------------------------------

/**
   LVGL Widget class.

   This class exposes the LVGL drawing API inside a DGL Widget.

   This class will take care of setting up LVGL for drawing,
   and also user input, resizes and everything in between.
 */
template <class BaseWidget>
class LVGLWidget : public BaseWidget,
                   public IdleCallback
{
public:
   /**
      Constructor for a LVGLSubWidget.
    */
    explicit LVGLWidget(Widget* parentGroupWidget);

   /**
      Constructor for a LVGLTopLevelWidget.
    */
    explicit LVGLWidget(Window& windowToMapTo);

   /**
      Constructor for a LVGLStandaloneWindow without transient parent window.
    */
    explicit LVGLWidget(Application& app);

   /**
      Constructor for a LVGLStandaloneWindow with transient parent window.
    */
    explicit LVGLWidget(Application& app, Window& transientParentWindow);

   /**
      Destructor.
    */
    ~LVGLWidget() override;

protected:
    void idleCallback() override;
    void onDisplay() override;
    bool onKeyboard(const Widget::KeyboardEvent& event) override;
    bool onMouse(const Widget::MouseEvent& event) override;
    bool onMotion(const Widget::MotionEvent& event) override;
    bool onScroll(const Widget::ScrollEvent& event) override;
    void onResize(const Widget::ResizeEvent& event) override;

    struct PrivateData;
    PrivateData* const lvglData;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LVGLWidget)
};

typedef LVGLWidget<SubWidget> LVGLSubWidget;
typedef LVGLWidget<TopLevelWidget> LVGLTopLevelWidget;
typedef LVGLWidget<StandaloneWindow> LVGLStandaloneWindow;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
