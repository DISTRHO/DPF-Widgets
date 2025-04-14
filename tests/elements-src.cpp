/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2022 Filipe Coelho <falktx@falktx.com>
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

// Elements is quite large, build it separately
#define ELEMENTS_HOST_UI_LIBRARY_DPF 1
#include "../cairo/Elements/src/element/button.cpp"
#include "../cairo/Elements/src/element/child_window.cpp"
#include "../cairo/Elements/src/element/composite.cpp"
#include "../cairo/Elements/src/element/dial.cpp"
#include "../cairo/Elements/src/element/dynamic_list.cpp"
#include "../cairo/Elements/src/element/element.cpp"
#include "../cairo/Elements/src/element/floating.cpp"
#include "../cairo/Elements/src/element/flow.cpp"
#include "../cairo/Elements/src/element/gallery/button.cpp"
#include "../cairo/Elements/src/element/gallery/check_box.cpp"
#include "../cairo/Elements/src/element/gallery/icon_button.cpp"
#include "../cairo/Elements/src/element/gallery/menu.cpp"
#include "../cairo/Elements/src/element/gallery/radio_button.cpp"
#include "../cairo/Elements/src/element/gallery/tab.cpp"
#include "../cairo/Elements/src/element/gallery/thumbwheel.cpp"
#include "../cairo/Elements/src/element/grid.cpp"
#include "../cairo/Elements/src/element/image.cpp"
#include "../cairo/Elements/src/element/label.cpp"
#include "../cairo/Elements/src/element/layer.cpp"
#include "../cairo/Elements/src/element/menu.cpp"
#include "../cairo/Elements/src/element/misc.cpp"
#include "../cairo/Elements/src/element/popup.cpp"
#include "../cairo/Elements/src/element/port.cpp"
#include "../cairo/Elements/src/element/progress_bar.cpp"
#include "../cairo/Elements/src/element/proxy.cpp"
#include "../cairo/Elements/src/element/slider.cpp"
#include "../cairo/Elements/src/element/text.cpp"
#include "../cairo/Elements/src/element/thumbwheel.cpp"
#include "../cairo/Elements/src/element/tooltip.cpp"
#include "../cairo/Elements/src/support/canvas.cpp"
#include "../cairo/Elements/src/support/draw_utils.cpp"
#include "../cairo/Elements/src/support/font.cpp"
#include "../cairo/Elements/src/support/glyphs.cpp"
#include "../cairo/Elements/src/support/pixmap.cpp"
#include "../cairo/Elements/src/support/receiver.cpp"
#include "../cairo/Elements/src/support/rect.cpp"
#include "../cairo/Elements/src/support/text_utils.cpp"
#include "../cairo/Elements/src/support/theme.cpp"
#include "../cairo/Elements/src/support/resource_paths.cpp"
#include "../cairo/Elements/src/view.cpp"
// must be last, for 'range' name ambiguity
#include "../cairo/Elements/src/element/tile.cpp"
