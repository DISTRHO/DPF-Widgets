/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2025 Filipe Coelho <falktx@falktx.com>
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

#include "Application.hpp"
#include "Layout.hpp"
#include "../opengl/DarkBreeze.cpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

class BreezeStandaloneWindow : public StandaloneWindow
{
    DarkBreeze breeze;
    ScopedPointer<DarkBreezeButton> button1;
    ScopedPointer<DarkBreezeButton> button2;
    ScopedPointer<DarkBreezeButton> button3;
    HorizontalLayout layout;

public:
    BreezeStandaloneWindow(Application& app)
        : StandaloneWindow(app),
          breeze(*this)
    {
        const double scaleFactor = getScaleFactor();
        setGeometryConstraints(500 * scaleFactor, 80 * scaleFactor, false);
        setSize(580 * scaleFactor, 80 * scaleFactor);
        setResizable(true);
        setTitle("Breeze");

        button1 = new DarkBreezeButton(&breeze);
        button1->setLabel("fixed size button");

        button2 = new DarkBreezeButton(&breeze);
        button2->setLabel("expanding button 1");

        button3 = new DarkBreezeButton(&breeze);
        button3->setLabel("expanding button 2");

        layout.widgets.push_back({button1, Fixed});
        layout.widgets.push_back({button2, Expanding});
        layout.widgets.push_back({button3, Expanding});
        layout.setAbsolutePos(0, 0, 6);
        layout.setSize(getWidth(), 6);

        focus();
    }

protected:
    void onFocus(bool focus, CrossingMode mode) override
    {
        d_stdout("%d %d", focus, mode);
        breeze.setActive(focus);
    }

    void onReshape(uint width, uint height) override
    {
        StandaloneWindow::onReshape(width, height);
        layout.setSize(width, 6);
        layout.setAbsolutePos(0, 0, 6);
    }
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

int main(int, char**)
{
    USE_NAMESPACE_DGL;

    Application app;
    BreezeStandaloneWindow win(app);
    win.show();
    app.exec();

    return 0;
}
