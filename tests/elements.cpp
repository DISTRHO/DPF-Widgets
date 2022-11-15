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
#define ELEMENTS_SKIP_IMPLEMENTATION

#include "../cairo/Elements.cpp"

START_NAMESPACE_DGL

using namespace cycfi::elements;

class ElementsDemo : public ElementsStandaloneWindow
{
    // Main window background color
    static constexpr auto bkd_color = rgba(35, 35, 37, 255);
    box_element background = box(bkd_color);

    using slider_ptr = std::shared_ptr<basic_slider_base>;
    slider_ptr hsliders[3];
    slider_ptr vsliders[3];

    using dial_ptr = std::shared_ptr<dial_base>;
    dial_ptr dials[3];

    template <bool is_vertical>
    auto make_markers()
    {
        auto track = basic_track<5, is_vertical>();
        return slider_labels<10>(
            slider_marks<40>(track),         // Track with marks
            0.8,                             // Label font size (relative size)
            "0", "1", "2", "3", "4",         // Labels
            "5", "6", "7", "8", "9", "10"
        );
    }

    auto make_hslider(int index)
    {
        hsliders[index] = share(slider(
            basic_thumb<25>(),
            make_markers<false>(),
            (index + 1) * 0.25
        ));
        return align_middle(hmargin({ 20, 20 }, hold(hsliders[index])));
    }

    auto make_hsliders()
    {
        return hmin_size(300,
            vtile(
                make_hslider(0),
                make_hslider(1),
                make_hslider(2)
            )
        );
    }

    auto make_vslider(int index)
    {
        vsliders[index] = share(slider(
            basic_thumb<25>(),
            make_markers<true>(),
            (index + 1) * 0.25
        ));
        return align_center(vmargin({ 20, 20 }, hold(vsliders[index])));
    }

    auto make_vsliders()
    {
        return hmin_size(300,
            htile(
                make_vslider(0),
                make_vslider(1),
                make_vslider(2)
            )
        );
    }

    auto make_dial(int index)
    {
        dials[index] = share(
            dial(
                radial_marks<20>(basic_knob<50>()),
                (index + 1) * 0.25
            )
        );

        auto markers = radial_labels<15>(
            hold(dials[index]),
            0.7,                                   // Label font size (relative size)
            "0", "1", "2", "3", "4",               // Labels
            "5", "6", "7", "8", "9", "10"
        );

        return align_center_middle(markers);
    }

    auto make_dials()
    {
        return hmargin(20,
            vtile(
                make_dial(0),
                make_dial(1),
                make_dial(2)
            )
        );
    }

    auto make_controls()
    {
        return
            margin({ 20, 10, 20, 10 },
                vmin_size(400,
                    htile(
                    margin({ 20, 20, 20, 20 }, pane("Vertical Sliders", make_vsliders(), 0.8f)),
                    margin({ 20, 20, 20, 20 }, pane("Horizontal Sliders", make_hsliders(), 0.8f)),
                    hstretch(0.5, margin({ 20, 20, 20, 20 }, pane("Knobs", make_dials(), 0.8f)))
                    )
                )
            );
    }

    void link_control(int index)
    {
        vsliders[index]->on_change =
            [index, this](double val)
            {
                hsliders[index]->slider_base::value(val);
                dials[index]->dial_base::value(val);
                refresh(*hsliders[index]);
                refresh(*dials[index]);
            };

        hsliders[index]->on_change =
            [index, this](double val)
            {
                vsliders[index]->slider_base::value(val);
                dials[index]->dial_base::value(val);
                refresh(*vsliders[index]);
                refresh(*dials[index]);
            };

        dials[index]->on_change =
            [index, this](double val)
            {
                vsliders[index]->slider_base::value(val);
                hsliders[index]->slider_base::value(val);
                refresh(*vsliders[index]);
                refresh(*hsliders[index]);
            };
    }

    void link_controls()
    {
        link_control(0);
        link_control(1);
        link_control(2);
    }

public:
    ElementsDemo(Application& app)
        : ElementsStandaloneWindow(app)
    {
        content(
            make_controls(),
            background
        );

        link_controls();
    }
};

END_NAMESPACE_DGL

int main(int, char**)
{
    USE_NAMESPACE_DGL;

    Application app;
    ElementsDemo win(app);
    win.setSize(900*win.getScaleFactor(), 500*win.getScaleFactor());
    win.setResizable(true);
    win.setTitle("Elements with DPF \\o/");
    win.show();
    app.exec();

    return 0;
}
