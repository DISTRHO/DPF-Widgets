/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2021 Filipe Coelho <falktx@falktx.com>
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
#include "StandaloneWindow.hpp"

// ImGui is quite large, build it separately
#define IMGUI_SKIP_IMPLEMENTATION

#include "../generic/ResizeHandle.hpp"
#include "../opengl/Blendish.cpp"
#include "../opengl/DearImGui.cpp"

#include <vector>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------
// Blendish demo

class BlendishDemo : public BlendishSubWidgetSharedContext
{
    BlendishToolButton toolButton1;
    BlendishToolButton toolButton2;
    BlendishToolButton toolButton3;

    BlendishNumberField numberFieldX;
    BlendishNumberField numberFieldY;
    BlendishNumberField numberFieldW;
    BlendishNumberField numberFieldH;

    BlendishCheckBox checkBox1;
    BlendishCheckBox checkBox2;
    BlendishCheckBox checkBox3;

    BlendishComboBox comboBox;

    BlendishMenu menu;

public:
    BlendishDemo(Widget* const parent)
        : BlendishSubWidgetSharedContext(parent),
          toolButton1(this),
          toolButton2(this),
          toolButton3(this),
          numberFieldX(this),
          numberFieldY(this),
          numberFieldW(this),
          numberFieldH(this),
          checkBox1(this),
          checkBox2(this),
          checkBox3(this),
          comboBox(this),
          menu(this)
    {
        toolButton1.setLabel("A toolbutton");
        toolButton2.setLabel("Another toolbutton");
        toolButton3.setLabel("The final toolbutton");
        groupBlendishWidgets(true, &toolButton1, &toolButton2, &toolButton3, nullptr);

        numberFieldX.setAbsoluteY(toolButton1.getAbsoluteY() + toolButton1.getHeight() + 5);
        numberFieldX.setLabel("X");
        numberFieldY.setLabel("Y");
        numberFieldW.setLabel("Width");
        numberFieldH.setLabel("Height");
        groupBlendishWidgets(false, &numberFieldX, &numberFieldY, &numberFieldW, &numberFieldH, nullptr);

        checkBox1.setAbsoluteY(numberFieldH.getAbsoluteY() + numberFieldH.getHeight() + 5);
        checkBox1.setLabel("A checkbox");
        checkBox2.setLabel("Another one");
        checkBox3.setLabel("Another one!");
        groupBlendishWidgets(false, &checkBox1, &checkBox2, &checkBox3, nullptr);

        comboBox.setAbsoluteX(toolButton2.getAbsoluteX());
        comboBox.setAbsoluteY(numberFieldX.getAbsoluteY());
        comboBox.setDefaultLabel("This is a combo-box");
        comboBox.addMenuItem("First item");
        comboBox.addMenuItem("Second item");
        comboBox.addMenuItem("Third item");
        comboBox.addMenuItem("Fourth item");

        /*
        menu.setAbsoluteX(numberFieldH.getAbsoluteX());
        menu.setAbsoluteY(checkBox3.getAbsoluteY() + checkBox3.getHeight() + 5);
        */
        menu.setAbsoluteX(toolButton2.getAbsoluteX());
        menu.setAbsoluteY(numberFieldW.getAbsoluteY());
        menu.setLabel("This is a menu, and I am a title");
        menu.addMenuItem("First item of the menu");
        menu.addMenuItem("Short second");
        menu.addMenuItem("Thirsty third");
        menu.addMenuItem("Furious Four");
        menu.addMenuItem("Fast Five");
    }
};

// --------------------------------------------------------------------------------------------------------------------
// Dear ImGui demo

class DearImGuiDemo : public ImGuiSubWidget
{
public:
    DearImGuiDemo(Widget* const parent)
        : ImGuiSubWidget(parent)
    {
    }

protected:
    void onImGuiDisplay()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(getWidth(), getHeight()));
//         ImGui::ShowAboutWindow();
        ImGui::ShowDemoWindow();
        return;

        if (ImGui::Begin("Simple gain", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            static char aboutText[256] =
                "This is a demo plugin made with ImGui.\n";
            ImGui::InputTextMultiline("About", aboutText, sizeof(aboutText));

            static float gain = 0.0f;
            ImGui::SliderFloat("Gain (dB)", &gain, -90.0f, 30.0f);
        }
        ImGui::End();
    }
};

// --------------------------------------------------------------------------------------------------------------------
// Main Widgets Demo Window, having a left-side tab-like widget and main area for current widget

class WidgetsDemoWindow : public StandaloneWindow
{
    static const uint kMarginBase    = 8;
    static const uint kMarginContent = kMarginBase + 2;

    static const uint kMinWindowWidth  = 720;
    static const uint kMinWindowHeight = 560;

    ResizeHandle resizeHandle;
    struct WidgetAndOffset {
        SubWidget* widget;
        uint offset;
    };
    std::vector<WidgetAndOffset> widgets;

    template <class W>
    W* createAndAddWidgetOfType(const uint margin, const uint yOffset)
    {
        W* const w = new W((TopLevelWidget*)this);
        widgets.push_back({w, yOffset});

        w->setAbsolutePos(margin, margin + yOffset);
        w->setSize(getWidth()-margin*2, getHeight()-margin*2-yOffset);

        return w;
    }

public:
    WidgetsDemoWindow(Application& app)
        : StandaloneWindow(app),
          resizeHandle(this)
    {
        const double scaleFactor = getScaleFactor();
        const uint margin = kMarginContent * scaleFactor;

        setGeometryConstraints(kMinWindowWidth * scaleFactor, kMinWindowHeight * scaleFactor, false, false);
        setSize(kMinWindowWidth * scaleFactor, kMinWindowHeight * scaleFactor);
        setResizable(true);
        setTitle("DPF Widgets Demo");

        const ScopedGraphicsContext sgc(*this);
        createAndAddWidgetOfType<BlendishDemo>(margin, 0);
        createAndAddWidgetOfType<DearImGuiDemo>(margin, 200);
    }

protected:
    void onDisplay() override
    {
        const GraphicsContext& context(getGraphicsContext());
        const double scaleFactor = getScaleFactor();
        const uint margin = kMarginBase * scaleFactor;
        const uint width  = getWidth();
        const uint height = getHeight();

        Color::fromHTML("#5680c2").setFor(context);
        Rectangle<uint>(0, 0, width, margin).draw(context);
        Rectangle<uint>(0, 0, margin, height).draw(context);
        Rectangle<uint>(0, height-margin, width, height).draw(context);
        Rectangle<uint>(width-margin, 0, width, height).draw(context);

        Color::fromHTML("#727272").setFor(context);
        Rectangle<uint>(margin, margin, width-margin*2, height-margin*2).draw(context);
    }

    void onReshape(uint width, uint height) override
    {
        StandaloneWindow::onReshape(width, height);

        const double scaleFactor = getScaleFactor();
        const uint margin = kMarginContent * scaleFactor;

        for (WidgetAndOffset& wo : widgets)
        {
            const uint yOffset = wo.offset * scaleFactor;
            wo.widget->setSize(width-margin*2, height-margin*2-yOffset);
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL

int main(int, char**)
{
    USE_NAMESPACE_DGL;

    Application app;
    WidgetsDemoWindow win(app);
    win.show();
    app.exec();

    return 0;
}
