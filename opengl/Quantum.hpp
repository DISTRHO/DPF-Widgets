/*
 * Quanta-inspired widgets for DPF
 * Copyright (C) 2022-2025 Filipe Coelho <falktx@falktx.com>
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

static inline constexpr
float normalizedLevelMeterValue(const float db)
{
    return (
        db < -70.f ? 0.f :
        db < -60.f ? (db + 70.f) * 0.25f :
        db < -50.f ? (db + 60.f) * 0.50f +  2.5f :
        db < -40.f ? (db + 50.f) * 0.75f +  7.5f :
        db < -30.f ? (db + 40.f) * 1.50f + 15.0f :
        db < -20.f ? (db + 30.f) * 2.00f + 30.0f :
        db <   0.f ? (db + 20.f) * 2.50f + 50.0f :
        100.f
    ) / 100.f;
}

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
    uint knobIndicatorSize = 4;
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
    // knob ring color
    Color knobRingColor = levelMeterColor;
    // alternative knob rim color
    Color knobAlternativeRingColor = levelMeterAlternativeColor;
    // background color for widgets, e.g. slider line and knob padding, typically dark
    Color widgetBackgroundColor = Color::fromHTML("#141414");
    // active color for widgets, e.g. pressed button and knob body
    Color widgetActiveColor = Color::fromHTML("#578079");
    // alternative color for widgets, similar to the active just an alternative color
    Color widgetAlternativeColor = Color::fromHTML("#5f64f6");
    // foreground color for widgets, e.g. slider handle and knob indicator, typically light
    Color widgetForegroundColor = Color::fromHTML("#dcdcdc");
    // window background, typically lighter than widget background
    Color windowBackgroundColor = Color::fromHTML("#3d3d3d");
    // text color, main / brightest tone
    Color textLightColor = Color::fromHTML("#ffffff");
    // text color, mid brightness tone
    Color textMidColor = Color::fromHTML("#b3b3b3");
    // text color, dark tone
    Color textDarkColor = Color::fromHTML("#787878");
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
    Size<uint> radioSwitch;
    Size<uint> gainReductionMeter;
    Size<uint> knob;
    Size<uint> mixerSlider;
    Size<uint> stereoLevelMeter;
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
          radioSwitch(theme.fontSize * 3 + theme.borderSize * 2,
                      theme.fontSize * 1.333 + theme.borderSize * 2),
          gainReductionMeter(theme.textHeight * 2,
                             theme.textHeight * 4),
          knob(theme.textHeight * 3 / 2,
               theme.textHeight * 3 / 2),
          mixerSlider(theme.textHeight * 2,
                      theme.textHeight * 4),
          stereoLevelMeter(theme.textHeight * 2 + theme.borderSize * 2,
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
    Color backgroundColor = theme.widgetActiveColor;
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
    uint customFontSize = 0;
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

    inline uint getCustomFontSize() const noexcept
    {
        return customFontSize;
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
    void setCustomFontSize(uint size, bool adjustSizeNow = true);
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

class QuantumRadioSwitch : public NanoSubWidget,
                           public ButtonEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetActiveColor;

public:
    explicit QuantumRadioSwitch(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumRadioSwitch(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumRadioSwitch() override;

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    void adjustSize();
    void setBackgroundColor(Color color);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumRadioSwitch)
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

template<bool small>
class AbstractQuantumKnob : public NanoSubWidget,
                            public KnobEventHandler
{
public:
    enum Orientation {
        LeftToRight,
        CenterToSides,
    };

public:
    explicit AbstractQuantumKnob(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit AbstractQuantumKnob(NanoSubWidget* parent, const QuantumTheme& theme);
    ~AbstractQuantumKnob() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    inline Orientation getOrientation() const noexcept
    {
        return orientation;
    }

    inline Color getRingColor() const noexcept
    {
        return ringColor;
    }

    void setLabel(const char* label);
    void setOrientation(Orientation orientation);
    void setSideLabels(const char* label1, const char* label2);
    void setSideLabelsFontSize(uint fontSize);
    void setUnitLabel(const char* unitLabel);
    void setValueFontSize(uint fontSize);

    void setRingColor(Color color);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

private:
    const QuantumTheme& theme;
    Orientation orientation = LeftToRight;
    Color ringColor = theme.knobRingColor;
    char* label = nullptr;
    char* unitLabel = nullptr;
    char* sidelabels[2] = { nullptr, nullptr };
    uint sidelabelsFontSize = theme.fontSize;
    uint valueFontSize = small ? theme.fontSize * 0.75 : theme.fontSize * 2;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractQuantumKnob)
};

typedef AbstractQuantumKnob<false> QuantumKnob;
typedef AbstractQuantumKnob<true> QuantumSmallKnob;

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
template<bool withValue>
class AbstractQuantumGainReductionMeter : public NanoSubWidget
{
    const QuantumTheme& theme;
    bool enabled = true;
    char* label;
    float value = 0.f;

public:
    explicit AbstractQuantumGainReductionMeter(NanoSubWidget* parent, const QuantumTheme& theme);
    ~AbstractQuantumGainReductionMeter() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    inline bool isEnabled() const noexcept
    {
        return enabled;
    }

    void setEnabled(bool enabled);
    void setLabel(const char* label);
    void setValue(float value);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AbstractQuantumGainReductionMeter)
};

typedef AbstractQuantumGainReductionMeter<false> QuantumGainReductionMeter;
typedef AbstractQuantumGainReductionMeter<true> QuantumGainReductionMeterWithValue;

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
    void setValueCentered(bool centered);
    void setValueFormat(const char* format);

protected:
    void onNanoDisplay() override;

    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetAlternativeColor;
    float maximum = 1.f;
    float minimum = 0.f;
    Orientation orientation = LeftToRight;
    Color textColor = theme.textLightColor;
    char* unitLabel = nullptr;
    float value = 0.f;
    bool valueCentered = false;
    char* valueFormat = nullptr;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueSlider : public NanoSubWidget,
                           public KnobEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetActiveColor;
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
    bool onScroll(const ScrollEvent& ev) override;

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

class QuantumStereoLevelMeter : public NanoSubWidget,
                                public IdleCallback
{
    Application& app;
    const QuantumTheme& theme;
    bool enabled = true;
    float valueL = 0.f;
    float valueR = 0.f;
    float minimum = 0.f;
    float maximum = 1.f;
    float falloffL = 0.f;
    float falloffR = 0.f;
    double timeL = 0.0;
    double timeR = 0.0;
    double lastTimeL = 0.0;
    double lastTimeR = 0.0;
    char* topLabel = nullptr;

public:
    explicit QuantumStereoLevelMeter(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumStereoLevelMeter(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumStereoLevelMeter() override;

    inline bool isEnabled() const noexcept
    {
        return enabled;
    }

    inline const char* getTopLabel() const noexcept
    {
        return topLabel;
    }

    void setEnabled(bool enabled);
    void setRange(float min, float max);
    void setTopLabel(const char* label);
    void setValueL(float value);
    void setValueR(float value);
    void setValues(float l, float r);

protected:
    void onNanoDisplay() override;
    void idleCallback() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumStereoLevelMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumStereoLevelMeterWithLUFS : public NanoSubWidget,
                                        public IdleCallback
{
    Application& app;
    const QuantumTheme& theme;
    bool enabled = true;
    float valueL = 0.f;
    float valueR = 0.f;
    float valueLimiter = 0.f;
    float valueLufs = 0.f;
    float minimum = 0.f;
    float maximum = 1.f;
    float falloffL = 0.f;
    float falloffR = 0.f;
    double timeL = 0.0;
    double timeR = 0.0;
    double lastTimeL = 0.0;
    double lastTimeR = 0.0;
    char* topLabel = nullptr;

public:
    explicit QuantumStereoLevelMeterWithLUFS(NanoTopLevelWidget* parent, const QuantumTheme& theme);
    explicit QuantumStereoLevelMeterWithLUFS(NanoSubWidget* parent, const QuantumTheme& theme);
    ~QuantumStereoLevelMeterWithLUFS() override;

    inline bool isEnabled() const noexcept
    {
        return enabled;
    }

    inline const char* getTopLabel() const noexcept
    {
        return topLabel;
    }

    void setEnabled(bool enabled);
    void setRange(float min, float max);
    void setTopLabel(const char* label);
    void setValueL(float value);
    void setValueR(float value);
    void setValueLimiter(float value);
    void setValueLufs(float value);
    void setValues(float l, float r, float limiter, float lufs);

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
