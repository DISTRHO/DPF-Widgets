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
        return fs::path("/Users/falktx/Source/DISTRHO/DPF-Widgets/cairo/elements-git-src/resources/fonts");
    }

    // TODO make templated
    struct host_view {
        DGL_NAMESPACE::ElementsStandaloneWindow* self;
        point last_cursor_pos;
    };

    base_view::base_view(extent /* size_ */)
        : _view(new host_view)
    {
    }

    // base_view::base_view(host_window_handle h)
    //     : base_view(new host_view)
    // {
    //     // TODO
    //     // elements::make_view(*this, get_window(*h));
    // }

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
        return { 400, 400 };
      // return { float(_view->self->getWidth()), float(_view->self->getHeight()) };
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
        // FIXME
        _view->self->repaint();
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
    host()->self = this;
    addIdleCallback(this);
}

template <class BaseWidget>
ElementsWidget<BaseWidget>::~ElementsWidget()
{
    BaseWidget::removeIdleCallback(this);
}

template <class BaseWidget>
void ElementsWidget<BaseWidget>::onDisplay()
{
    auto cr = static_cast<const CairoGraphicsContext&>(BaseWidget::getGraphicsContext()).handle;
    //double left, top, right, bottom;
    //cairo_clip_extents(cr, &left, &top, &right, &bottom);

    //cycfi::elements::view::draw(cr, cycfi::elements::rect{ float(left), float(top), float(right), float(bottom) });
    cycfi::elements::view::draw(cr,{0,0,(float)BaseWidget::getWidth(),(float)BaseWidget::getHeight()});
}

template <class BaseWidget>
void ElementsWidget<BaseWidget>::idleCallback()
{
    cycfi::elements::view::poll();
    BaseWidget::repaint();
}

template class ElementsWidget<StandaloneWindow>;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------
