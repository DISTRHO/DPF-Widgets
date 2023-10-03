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

#include "Elements.hpp"

#ifdef DISTRHO_OS_MAC
# define Point MacPoint
# define Size MacSize
# include <cairo-quartz.h>
# undef Point
# undef Size
#endif

#ifndef ELEMENTS_SKIP_IMPLEMENTATION
#include "Elements/src/element/button.cpp"
#include "Elements/src/element/child_window.cpp"
#include "Elements/src/element/composite.cpp"
#include "Elements/src/element/dial.cpp"
#include "Elements/src/element/dynamic_list.cpp"
#include "Elements/src/element/element.cpp"
#include "Elements/src/element/floating.cpp"
#include "Elements/src/element/flow.cpp"
#include "Elements/src/element/gallery/button.cpp"
#include "Elements/src/element/gallery/check_box.cpp"
#include "Elements/src/element/gallery/icon_button.cpp"
#include "Elements/src/element/gallery/menu.cpp"
#include "Elements/src/element/gallery/radio_button.cpp"
#include "Elements/src/element/gallery/tab.cpp"
#include "Elements/src/element/gallery/thumbwheel.cpp"
#include "Elements/src/element/grid.cpp"
#include "Elements/src/element/image.cpp"
#include "Elements/src/element/label.cpp"
#include "Elements/src/element/layer.cpp"
#include "Elements/src/element/menu.cpp"
#include "Elements/src/element/misc.cpp"
#include "Elements/src/element/popup.cpp"
#include "Elements/src/element/port.cpp"
#include "Elements/src/element/progress_bar.cpp"
#include "Elements/src/element/proxy.cpp"
#include "Elements/src/element/slider.cpp"
#include "Elements/src/element/text.cpp"
#include "Elements/src/element/thumbwheel.cpp"
#include "Elements/src/element/tooltip.cpp"
#include "Elements/src/support/canvas.cpp"
#include "Elements/src/support/draw_utils.cpp"
#include "Elements/src/support/font.cpp"
#include "Elements/src/support/glyphs.cpp"
#include "Elements/src/support/pixmap.cpp"
#include "Elements/src/support/receiver.cpp"
#include "Elements/src/support/rect.cpp"
#include "Elements/src/support/text_utils.cpp"
#include "Elements/src/support/theme.cpp"
#include "Elements/src/support/resource_paths.cpp"
#include "Elements/src/view.cpp"
// must be last, for 'range' name ambiguity
#include "Elements/src/element/tile.cpp"
#endif

// --------------------------------------------------------------------------------------------------------------------

namespace cycfi { namespace elements
{
    std::string clipboard()
    {
        return {};
    }

    void clipboard(std::string const& text)
    {
    }

    void set_cursor(cursor_type type)
    {
    }

    // needed only for macOS?
    fs::path get_user_fonts_directory()
    {
        return fs::path("~/.fonts");
    }

    point scroll_direction()
    {
       #ifdef DISTRHO_OS_MAC
        // float dir = [[[NSUserDefaults standardUserDefaults] objectForKey:@"com.apple.swipescrolldirection"] boolValue]? +1.0f : -1.0f;
        // return { dir, dir };
       #else
       #endif

        return { 1.f, 1.f };
    }

    // TODO make templated
    struct host_view {
        DGL_NAMESPACE::Widget* self;
        mouse_button last_button = {};
        point last_cursor_pos = {};
        uint last_click_count = 0;
        uint last_click_time = 0;
        uint last_scroll_time = 0;
        // FIXME for template
        // void repaint(DGL_NAMESPACE::Rectangle<uint>&);
    };

    base_view::base_view(extent /* size_ */)
        : base_view(new host_view)
    {
    }

    base_view::base_view(host_view_handle h)
        : _view(h)
    {
    }

    base_view::~base_view()
    {
        delete _view;
    }

    point base_view::cursor_pos() const
    {
        return _view->last_cursor_pos;
    }

    elements::extent base_view::size() const
    {
        return { float(_view->self->getWidth()), float(_view->self->getHeight()) };
    }

    float base_view::hdpi_scale() const
    {
        return 1.0f; // ??
    }

    void base_view::refresh()
    {
        _view->self->repaint();
    }

    void base_view::refresh(rect area)
    {
        // FIXME must be template based
        _view->self->repaint();
        // _view->self->repaint(DGL_NAMESPACE::Rectangle<uint>());
    }
}}

// --------------------------------------------------------------------------------------------------------------------

#include "Elements.hpp"
#include "Application.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
void ElementsWidget<BaseWidget>::idleCallback()
{
    cycfi::elements::view::poll();
}

template <class BaseWidget>
void ElementsWidget<BaseWidget>::onDisplay()
{
    auto cr = static_cast<const CairoGraphicsContext&>(BaseWidget::getGraphicsContext()).handle;

    double left, top, right, bottom;
    cairo_clip_extents(cr, &left, &top, &right, &bottom);

    cycfi::elements::view::draw(cr, cycfi::elements::rect{ float(left), float(top), float(right), float(bottom) });
}

template <class BaseWidget>
bool ElementsWidget<BaseWidget>::onMouse(const Widget::MouseEvent& event)
{
    if (BaseWidget::onMouse(event))
        return true;

    using namespace cycfi::elements;
    host_view* const host_view_h = host();
    const float scaleFactor = BaseWidget::getWindow().getScaleFactor();

    mouse_button btn = {};
    btn.down = event.press;

    switch (event.button)
    {
    case kMouseButtonLeft:
        btn.state = mouse_button::left;
        break;
    case kMouseButtonRight:
        btn.state = mouse_button::right;
        break;
    case kMouseButtonMiddle:
        btn.state = mouse_button::middle;
        break;
    default:
        btn.down = false;
        break;
    }

    if (btn.down)
    {
        if ((event.time - host_view_h->last_click_time) < 400) // FIXME less hardcoded
            ++host_view_h->last_click_count;
        else
            host_view_h->last_click_count = 1;
        host_view_h->last_click_time = event.time;
    }

    if (event.mod & kModifierShift)
        btn.modifiers |= mod_shift;
    if (event.mod & kModifierControl)
        btn.modifiers |= mod_control | mod_action; // ???
    if (event.mod & kModifierAlt)
        btn.modifiers |= mod_alt;
    if (event.mod & kModifierSuper)
        btn.modifiers |= mod_action;

    btn.num_clicks = host_view_h->last_click_count;
    btn.pos = { float(event.pos.getX()) / scaleFactor, float(event.pos.getY()) / scaleFactor };

    host_view_h->last_button = btn;
    host_view_h->last_cursor_pos = btn.pos;

    view::click(btn);
    return true;
}

template <class BaseWidget>
bool ElementsWidget<BaseWidget>::onMotion(const Widget::MotionEvent& event)
{
    if (BaseWidget::onMotion(event))
        return true;

    using namespace cycfi::elements;
    host_view* const host_view_h = host();
    const float scaleFactor = BaseWidget::getWindow().getScaleFactor();

    const point pos = { float(event.pos.getX()) / scaleFactor, float(event.pos.getY()) / scaleFactor };
    host_view_h->last_cursor_pos = pos;

    if (host_view_h->last_button.down)
    {
        host_view_h->last_button.pos = pos;
        view::drag(host_view_h->last_button);
    }
    else
    {
        view::cursor(host_view_h->last_cursor_pos, cursor_tracking::hovering);
    }

    return true;
}

template <class BaseWidget>
bool ElementsWidget<BaseWidget>::onScroll(const Widget::ScrollEvent& event)
{
    if (BaseWidget::onScroll(event))
        return true;

    using namespace cycfi::elements;
    host_view* const host_view_h = host();
    const float scaleFactor = BaseWidget::getWindow().getScaleFactor();

    static constexpr float _1s = 100;
    const float elapsed = std::max<float>(10.f, event.time - host_view_h->last_scroll_time);
    host_view_h->last_scroll_time = event.time;

    float dx = 0;
    float dy = 0;
    float step = _1s / elapsed;

    switch (event.direction)
    {
    case kScrollUp:
        dy = step;
        break;
    case kScrollDown:
        dy = -step;
        break;
    case kScrollLeft:
        dx = step;
        break;
    case kScrollRight:
        dx = -step;
        break;
    case kScrollSmooth:
        dx = event.delta.getX() / scaleFactor;
        dy = event.delta.getY() / scaleFactor;
        break;
    default:
        break;
    }

    view::scroll(
        { dx, dy },
        { float(event.pos.getX()) / scaleFactor, float(event.pos.getY()) / scaleFactor }
    );

    return true;
}

// --------------------------------------------------------------------------------------------------------------------

template <>
ElementsWidget<SubWidget>::ElementsWidget(Widget* const parentGroupWidget)
    : SubWidget(parentGroupWidget),
      view({400,400})
{
    host()->self = this;
    getApp().addIdleCallback(this);
}

template <>
ElementsWidget<SubWidget>::~ElementsWidget()
{
    getApp().removeIdleCallback(this);
}

template class ElementsWidget<SubWidget>;

// --------------------------------------------------------------------------------------------------------------------

template <>
ElementsWidget<TopLevelWidget>::ElementsWidget(Window& windowToMapTo)
    : TopLevelWidget(windowToMapTo),
      view({400,400})
{
    host()->self = this;
    addIdleCallback(this);
}

template <>
ElementsWidget<TopLevelWidget>::~ElementsWidget()
{
    removeIdleCallback(this);
}

template class ElementsWidget<TopLevelWidget>;

// --------------------------------------------------------------------------------------------------------------------

template <>
ElementsWidget<StandaloneWindow>::ElementsWidget(Application& app)
    : StandaloneWindow(app),
      view({400,400})
{
    host()->self = this;
    addIdleCallback(this);
}

template <>
ElementsWidget<StandaloneWindow>::ElementsWidget(Application& app, Window& transientParentWindow)
    : StandaloneWindow(app, transientParentWindow),
      view({400,400})
{
    host()->self = this;
    addIdleCallback(this);
}

template <>
ElementsWidget<StandaloneWindow>::~ElementsWidget()
{
    removeIdleCallback(this);
}

template class ElementsWidget<StandaloneWindow>;

// --------------------------------------------------------------------------------------------------------------------

ElementsStandaloneWindow::ElementsStandaloneWindow(Application& app)
    : ElementsWidget<StandaloneWindow>(app) {}

ElementsStandaloneWindow::ElementsStandaloneWindow(Application& app, Window& transientParentWindow)
    : ElementsWidget<StandaloneWindow>(app, transientParentWindow) {}

void ElementsStandaloneWindow::onFocus(bool focus, CrossingMode mode)
{
    using namespace cycfi::elements;

    if (focus)
    {
        view::begin_focus();
    }
    else
    {
        host()->last_button = {};
        view::end_focus();
    }
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------
