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

#ifndef ELEMENTS_SKIP_IMPLEMENTATION
#include "src/element/button.cpp"
#include "src/element/child_window.cpp"
#include "src/element/composite.cpp"
#include "src/element/dial.cpp"
#include "src/element/dynamic_list.cpp"
#include "src/element/element.cpp"
#include "src/element/floating.cpp"
#include "src/element/flow.cpp"
#include "src/element/gallery/button.cpp"
#include "src/element/gallery/check_box.cpp"
#include "src/element/gallery/icon_button.cpp"
#include "src/element/gallery/menu.cpp"
#include "src/element/gallery/radio_button.cpp"
#include "src/element/gallery/tab.cpp"
#include "src/element/gallery/thumbwheel.cpp"
#include "src/element/grid.cpp"
#include "src/element/image.cpp"
#include "src/element/label.cpp"
#include "src/element/layer.cpp"
#include "src/element/menu.cpp"
#include "src/element/misc.cpp"
#include "src/element/popup.cpp"
#include "src/element/port.cpp"
#include "src/element/progress_bar.cpp"
#include "src/element/proxy.cpp"
#include "src/element/slider.cpp"
#include "src/element/text.cpp"
#include "src/element/thumbwheel.cpp"
#include "src/element/tooltip.cpp"
#include "src/support/canvas.cpp"
#include "src/support/draw_utils.cpp"
#include "src/support/font.cpp"
#include "src/support/glyphs.cpp"
#include "src/support/pixmap.cpp"
#include "src/support/receiver.cpp"
#include "src/support/rect.cpp"
#include "src/support/text_utils.cpp"
#include "src/support/theme.cpp"
#include "src/support/resource_paths.cpp"
#include "src/view.cpp"
// must be last, for 'range' name ambiguity
#include "src/element/tile.cpp"
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
        return fs::path("/home/falktx/Source/DISTRHO/DPF-Widgets/cairo/Elements/.elements-git-src/resources/fonts");
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
        DGL_NAMESPACE::ElementsStandaloneWindow* self;
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

template <>
ElementsWidget<StandaloneWindow>::ElementsWidget(Application& app)
    : StandaloneWindow(app),
      view({400,400})
{
    addIdleCallback(this);
}

template <class BaseWidget>
ElementsWidget<BaseWidget>::~ElementsWidget()
{
    BaseWidget::removeIdleCallback(this);
}

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
    btn.pos = { float(event.pos.getX()), float(event.pos.getY()) };

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

    const point pos = { float(event.pos.getX()), float(event.pos.getY()) };
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
        dx = event.delta.getX();
        dy = event.delta.getY();
        break;
    default:
        break;
    }

    view::scroll(
        { dx, dy },
        { float(event.pos.getX()), float(event.pos.getY()) }
    );

    return true;
}

template class ElementsWidget<StandaloneWindow>;

// --------------------------------------------------------------------------------------------------------------------

ElementsStandaloneWindow::ElementsStandaloneWindow(Application& app)
    : ElementsWidget<StandaloneWindow>(app)
{
    host()->self = this;
}

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
