/*
 * Quanta-inspired widgets for DPF
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

#include "EventHandlers.hpp"
#include "Layout.hpp"
#include "NanoVG.hpp"
#include "SubWidget.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

struct QuantumTheme {
    // border size for widgets, e.g. button and knob outline border
    uint borderSize = 1;
    // padding for widgets, the space between each other when in a group
    uint padding = 2;
    // main font size, used for all text unless otherwise stated
    uint fontSize = 15;
    // height given to text labels and widgets that use text (without padding)
    uint textHeight = 20;
    // line size for widgets, e.g. slider line
    uint widgetLineSize = 2;
    // how much padding to give from window border to widgets
    uint windowPadding = borderSize + padding * 3;
    // how much extra width to give to text labels to compensate for rounding with >= 2x scaling factor
    uint textPixelRatioWidthCompensation = 0;
    // background color for level meter widgets
    Color levelMeterColor = Color::fromHTML("#4ca947");
    // alternative background color for level meter widgets
    Color levelMeterAlternativeColor = Color::fromHTML("#ffc400");
    // background color for widgets, e.g. slider line and knob padding, typically dark
    Color widgetBackgroundColor = Color::fromHTML("#141414");
    // default active color for widgets, e.g. pressed button and knob body
    Color widgetDefaultActiveColor = Color::fromHTML("#3f535a");
    // default alternative color for widgets, similar to the active just an alternative color
    Color widgetDefaultAlternativeColor = Color::fromHTML("#585338");
    // foreground color for widgets, e.g. slider handle and knob indicator, typically light
    Color widgetForegroundColor = Color::fromHTML("#8c8c8c");
    // window background, typically lighter than widget background
    Color windowBackgroundColor = Color::fromHTML("#3d3d3d");
    // text color, main / brightest tone
    Color textLightColor = Color::fromHTML("#ffffff");
    // text color, mid brightness tone
    Color textMidColor = Color::fromHTML("#b3b3b3");
    // text color, dark tone
    Color textDarkColor = Color::fromHTML("#8c8c8c");
    // text color, very dark tone
    Color textVeryDarkColor = Color::fromHTML("#383838");
};

struct QuantumMetrics
{
    Size<uint> button;
    Size<uint> frame;
    Size<uint> label;
    Size<uint> separator;
    Size<uint> switch_;
    Size<uint> knob;
    Size<uint> mixerSlider;
    Size<uint> stereoLevelMeterWithLufs;
    Size<uint> valueMeterHorizontal;
    Size<uint> valueMeterVertical;
    Size<uint> valueSlider;

    explicit QuantumMetrics(const QuantumTheme& theme) noexcept
        : button(theme.textHeight + theme.borderSize * 2,
                 theme.textHeight + theme.borderSize * 2),
          frame(theme.borderSize * 2 + theme.padding * 2,
                theme.borderSize * 2 + theme.padding * 2),
          label(theme.padding,
                theme.textHeight),
          separator(theme.textHeight,
                    theme.borderSize),
          switch_(theme.textHeight * 2 + theme.borderSize * 2,
                 theme.textHeight / 2 + theme.borderSize * 2),
          knob(theme.textHeight * 3 / 2,
                 theme.textHeight * 3 / 2),
          mixerSlider(theme.textHeight,
                      theme.textHeight * 4),
          stereoLevelMeterWithLufs(theme.textHeight * 2 + theme.borderSize * 4 + theme.widgetLineSize,
                                   theme.textHeight * 4),
          valueMeterHorizontal(theme.textHeight * 4,
                               theme.textHeight),
          valueMeterVertical(theme.textHeight,
                             theme.textHeight * 4),
          valueSlider(theme.textHeight * 4,
                      theme.textHeight)
    {
    }
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumButton : public NanoSubWidget,
                      public ButtonEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;
    char* label = nullptr;

public:
    explicit QuantumButton(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumButton(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumButton() override;

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void adjustSize();
    void setBackgroundColor(Color color);
    void setLabel(const char* label, bool adjustSizeNow = true);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumButton)
};

// --------------------------------------------------------------------------------------------------------------------

// alignment uses NanoVG::Align
class QuantumLabel : public NanoSubWidget
{
    const QuantumTheme& theme;
    uint alignment = ALIGN_LEFT|ALIGN_MIDDLE;
    char* label = nullptr;
    Color labelColor = theme.textLightColor;

public:
    explicit QuantumLabel(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumLabel(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumLabel() override;

    inline uint getAlignment() const noexcept
    {
        return alignment;
    }

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    inline Color getLabelColor() const noexcept
    {
        return labelColor;
    }

    void adjustSize();
    void setAlignment(uint alignment);
    void setLabel(const char* label, bool adjustSizeNow = true);
    void setLabelColor(Color color);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumLabel)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumSeparatorLine : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit QuantumSeparatorLine(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumSeparatorLine(NanoSubWidget* parent, const QuantumTheme& theme);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumSeparatorLine)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumSpacer : public SubWidget
{
public:
    explicit QuantumSpacer(TopLevelWidget* parent);
    explicit QuantumSpacer(SubWidget* parent);

protected:
    void onDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumSpacer)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumSwitch : public NanoSubWidget,
                      public ButtonEventHandler
{
    const QuantumTheme& theme;
    char* label = nullptr;

public:
    explicit QuantumSwitch(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumSwitch(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumSwitch() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void adjustSize();

    // width changes when called
    void setLabel(const char* label, bool adjustSizeNow = true);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumSwitch)
};

// --------------------------------------------------------------------------------------------------------------------

/*
class QuantumDualSidedSwitch : public NanoSubWidget,
                               public ButtonEventHandler
{
    const QuantumTheme& theme;
    char* labelLeft = nullptr;
    char* labelRight = nullptr;

public:
    explicit QuantumDualSidedSwitch(TopLevelWidget* parent, const QuantumTheme& theme);
    ~QuantumDualSidedSwitch() override;

    // width changes when called
    void setLabels(const char* left, const char* right);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumDualSidedSwitch)
};
*/

// --------------------------------------------------------------------------------------------------------------------

class QuantumKnob : public NanoSubWidget,
                    public KnobEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;

public:
    explicit QuantumKnob(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumKnob(NanoSubWidget* parent, const QuantumTheme& theme);

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    void setBackgroundColor(Color color);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumKnob)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumMixerSlider : public NanoSubWidget,
                           public KnobEventHandler
{
    const QuantumTheme& theme;

public:
    explicit QuantumMixerSlider(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumMixerSlider(NanoSubWidget* parent, const QuantumTheme& theme);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumMixerSlider)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueMeter : public NanoSubWidget
{
public:
    enum Orientation {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };

    explicit QuantumValueMeter(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumValueMeter(NanoSubWidget* parent, const QuantumTheme& theme);

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    inline Orientation getOrientation() const noexcept
    {
        return orientation;
    }

    inline Color getTextColor() const noexcept
    {
        return textColor;
    }

    inline const char* getUnitLabel() const noexcept
    {
        return unitLabel;
    }

    void setBackgroundColor(Color color);
    void setOrientation(Orientation orientation);
    void setRange(float min, float max);
    void setValue(float value);
    void setTextColor(Color color);
    void setUnitLabel(const char* label);

protected:
    void onNanoDisplay() override;

    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultAlternativeColor;
    float maximum = 1.f;
    float minimum = 0.f;
    Orientation orientation = LeftToRight;
    Color textColor = theme.textLightColor;
    char* unitLabel = nullptr;
    float value = 0.f;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueSlider : public NanoSubWidget,
                           public KnobEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;
    Color textColor = theme.textLightColor;
    char* unitLabel = nullptr;

public:
    explicit QuantumValueSlider(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumValueSlider(NanoSubWidget* parent, const QuantumTheme& theme);

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    inline Color getTextColor() const noexcept
    {
        return textColor;
    }

    inline const char* getUnitLabel() const noexcept
    {
        return unitLabel;
    }

    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setUnitLabel(const char* label);

    bool setValue(float value, bool sendCallback = false) noexcept override;

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueSlider)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumLevelMeter : public QuantumValueMeter
{
public:
    explicit QuantumLevelMeter(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumLevelMeter(NanoSubWidget* parent, const QuantumTheme& theme);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumLevelMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumStereoLevelMeterWithLUFS : public NanoSubWidget,
                                        public IdleCallback
{
    Application& app;
    const QuantumTheme& theme;
    float valueL = 0.f;
    float valueR = 0.f;
    float valueLufs = 0.f;
    float minimum = 0.f;
    float maximum = 1.f;
    float falloffL = 0.f;
    float falloffR = 0.f;
    double timeL = 0.0;
    double timeR = 0.0;
    double lastTime = 0.0;

public:
    explicit QuantumStereoLevelMeterWithLUFS(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumStereoLevelMeterWithLUFS(NanoSubWidget* parent, const QuantumTheme& theme);

    void setRange(float min, float max);
    void setValueL(float value);
    void setValueR(float value);
    void setValueLufs(float value);
    void setValues(float l, float r, float lufs);

protected:
    void onNanoDisplay() override;
    void idleCallback() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumStereoLevelMeterWithLUFS)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueMeter18 : public NanoSubWidget
{
    const QuantumTheme& theme;
    float values[18] = {};

public:
    explicit QuantumValueMeter18(TopLevelWidget* parent, const QuantumTheme& theme);

    void setValue(uint index, float value);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueMeter18)
};

// --------------------------------------------------------------------------------------------------------------------

template<class tMainWidget>
class AbstractQuantumFrame : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit AbstractQuantumFrame(TopLevelWidget* parent, const QuantumTheme& theme);
    explicit AbstractQuantumFrame(NanoSubWidget* parent, const QuantumTheme& theme);

    // publicly exposed for convenience, do not resize or reposition
    tMainWidget mainWidget;

    void adjustMainWidgetSize();

    inline uint getOffset() const noexcept
    {
        return offset;
    }

protected:
    void onNanoDisplay() override;
    void onPositionChanged(const PositionChangedEvent& ev) override;

private:
    uint offset = 0;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractQuantumFrame)
};

typedef AbstractQuantumFrame<char> QuantumFrame;
typedef AbstractQuantumFrame<QuantumLabel> QuantumFrameWithLabel;
typedef AbstractQuantumFrame<QuantumSwitch> QuantumFrameWithSwitch;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
