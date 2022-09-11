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
#include "Geometry.hpp"
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
    Color levelMeterColor = Color::fromHTML("#4a8179");
    // alternative background color for level meter widgets
    Color levelMeterAlternativeColor = Color::fromHTML("#ad68b9");
    // background color for widgets, e.g. slider line and knob padding, typically dark
    Color widgetBackgroundColor = Color::fromHTML("#141414");
    // default active color for widgets, e.g. pressed button and knob body
    Color widgetDefaultActiveColor = Color::fromHTML("#578079");
    // default alternative color for widgets, similar to the active just an alternative color
    Color widgetDefaultAlternativeColor = Color::fromHTML("#5f64f6");
    // foreground color for widgets, e.g. slider handle and knob indicator, typically light
    Color widgetForegroundColor = Color::fromHTML("#dcdcdc");
    // window background, typically lighter than widget background
    Color windowBackgroundColor = Color::fromHTML("#3d3d3d");
    // text color, main / brightest tone
    Color textLightColor = Color::fromHTML("#ffffff");
    // text color, mid brightness tone
    Color textMidColor = Color::fromHTML("#b3b3b3");
    // text color, dark tone
    Color textDarkColor = Color::fromHTML("#8c8c8c");
};

struct QuantumMetrics
{
    Size<uint> button;
    Size<uint> frame;
    Size<uint> label;
    Size<uint> separatorHorizontal;
    Size<uint> separatorVertical;
    Size<uint> smallSwitch;
    Size<uint> normalSwitch;
    Size<uint> gainReductionMeter;
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
          separatorHorizontal(theme.textHeight,
                              theme.borderSize),
          separatorVertical(theme.borderSize,
                            theme.textHeight),
          smallSwitch(theme.textHeight * 2 + theme.borderSize * 2,
                      theme.textHeight / 2 + theme.borderSize * 2),
          normalSwitch(theme.fontSize * 2 + theme.borderSize * 2,
                       theme.fontSize + theme.borderSize * 2),
          gainReductionMeter(theme.textHeight * 2,
                             theme.textHeight * 4),
          knob(theme.textHeight * 3 / 2,
                 theme.textHeight * 3 / 2),
          mixerSlider(theme.textHeight * 2,
                      theme.textHeight * 4),
          stereoLevelMeterWithLufs(theme.textHeight * 4 + theme.borderSize * 4,
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

struct QuantumThemeCallback {
    virtual ~QuantumThemeCallback() {}
    virtual void quantumThemeChanged(bool size, bool colors) = 0;
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumButton : public NanoSubWidget,
                      public ButtonEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;
    char* label = nullptr;
    bool labelHasNewLine = false;

public:
    explicit QuantumButton(NanoTopLevelWidget* parent, const QuantumTheme& theme);
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
    explicit QuantumLabel(NanoTopLevelWidget* parent, const QuantumTheme& theme);
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

template<bool horizontal>
class AbstractQuantumSeparatorLine : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit AbstractQuantumSeparatorLine(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit AbstractQuantumSeparatorLine(NanoSubWidget* parent, const QuantumTheme& theme);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractQuantumSeparatorLine)
};

typedef AbstractQuantumSeparatorLine<true> QuantumHorizontalSeparatorLine;
typedef AbstractQuantumSeparatorLine<false> QuantumVerticalSeparatorLine;

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

template<bool small>
class AbstractQuantumSwitch : public NanoSubWidget,
                              public ButtonEventHandler
{
    const QuantumTheme& theme;
    char* label = nullptr;

public:
    explicit AbstractQuantumSwitch(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit AbstractQuantumSwitch(NanoSubWidget* parent, const QuantumTheme& theme);
    ~AbstractQuantumSwitch() override;

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

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractQuantumSwitch)
};

typedef AbstractQuantumSwitch<false> QuantumSwitch;
typedef AbstractQuantumSwitch<true> QuantumSmallSwitch;

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
    explicit QuantumKnob(NanoTopLevelWidget* parent, const QuantumTheme& theme);
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

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumKnob)
};

// --------------------------------------------------------------------------------------------------------------------

// assumes -50 to 0 dB range
class QuantumMixerSlider : public NanoSubWidget,
                           public KnobEventHandler
{
    const QuantumTheme& theme;

public:
    explicit QuantumMixerSlider(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumMixerSlider(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumMixerSlider() override;

    Rectangle<int> getValueFieldAbsoluteArea() const noexcept;

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumMixerSlider)
};

// --------------------------------------------------------------------------------------------------------------------

// assumes -50 to 50 dB range
class QuantumGainReductionMeter : public NanoSubWidget
{
    const QuantumTheme& theme;
    float value = 0.f;

public:
    explicit QuantumGainReductionMeter(NanoSubWidget* parent, const QuantumTheme& theme);

    void setValue(float value);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumGainReductionMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueMeter : public NanoSubWidget
{
public:
    enum Orientation {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop,
        CenterToSides,
        MiddleToEdges // FIXME this needs a better name..
    };

    explicit QuantumValueMeter(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumValueMeter(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumValueMeter() override;

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
    explicit QuantumValueSlider(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumValueSlider(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumValueSlider() override;

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

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueSlider)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumLevelMeter : public QuantumValueMeter
{
public:
    explicit QuantumLevelMeter(NanoTopLevelWidget* parent, const QuantumTheme& theme);
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
    double lastTimeL = 0.0;
    double lastTimeR = 0.0;

public:
    explicit QuantumStereoLevelMeterWithLUFS(NanoTopLevelWidget* parent, const QuantumTheme& theme);
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

template<class tMainWidget>
class AbstractQuantumFrame : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit AbstractQuantumFrame(NanoTopLevelWidget* parent, const QuantumTheme& theme);
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
typedef AbstractQuantumFrame<QuantumSmallSwitch> QuantumFrameWithSwitch;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
