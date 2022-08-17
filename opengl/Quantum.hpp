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
#include "NanoVG.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

struct QuantumTheme {
    // border size for widgets, e.g. button and knob outline border
    int borderSize = 4;
    // padding for widgets, the space between each other when in a group
    int padding = 4;
    // main font size, used for all text unless otherwise stated
    int fontSize = 16;
    // height given to text labels and widgets that use text (without padding)
    int textHeight = 24;
    // line size for widgets, e.g. slider line
    int widgetLineSize = 4;
    // how much padding to give from window border to widgets
    int windowPadding = padding * 3;
    // background color for widgets, e.g. slider line and knob padding, typically dark
    Color widgetBackgroundColor = Color::fromHTML("#141414");
    // default active color for widgets, e.g. pressed button and knob body
    Color widgetDefaultActiveColor = Color::fromHTML("#3f535a");
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

// --------------------------------------------------------------------------------------------------------------------

class QuantumButton : public NanoSubWidget,
                      public ButtonEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;
    char* label = nullptr;

public:
    explicit QuantumButton(TopLevelWidget* parent, const QuantumTheme& theme);
    ~QuantumButton() override;

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void setBackgroundColor(Color color);
    void setLabel(const char* label, bool adjustWidth = true);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumButton)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumLabel : public NanoSubWidget
{
    const QuantumTheme& theme;
    char* label = nullptr;

public:
    explicit QuantumLabel(TopLevelWidget* parent, const QuantumTheme& theme);
    ~QuantumLabel() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void setLabel(const char* label, bool adjustWidth = true);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumLabel)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumSwitch : public NanoSubWidget,
                      public ButtonEventHandler
{
    const QuantumTheme& theme;
    char* label = nullptr;

public:
    explicit QuantumSwitch(TopLevelWidget* parent, const QuantumTheme& theme);
    ~QuantumSwitch() override;

    // width changes when called
    void setLabel(const char* label);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumSwitch)
};

// --------------------------------------------------------------------------------------------------------------------

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

// --------------------------------------------------------------------------------------------------------------------

class QuantumKnob : public NanoSubWidget,
                    public KnobEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;

public:
    explicit QuantumKnob(TopLevelWidget* parent, const QuantumTheme& theme);

    Color getBackgroundColor() const noexcept;
    void setBackgroundColor(Color color);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumKnob)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumLevelMeter : public NanoSubWidget,
                          public IdleCallback
{
    const QuantumTheme& theme;
    float value = 0.f;
    float falloff = 0.f;

public:
    explicit QuantumLevelMeter(TopLevelWidget* parent, const QuantumTheme& theme);

    inline float getNormalizedValue() const noexcept
    {
        return value;
    }

    bool setNormalizedValue(float value);

protected:
    void onNanoDisplay() override;
    void idleCallback() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumLevelMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumMixerSlider : public NanoSubWidget,
                           public KnobEventHandler
{
    const QuantumTheme& theme;

public:
    explicit QuantumMixerSlider(TopLevelWidget* parent, const QuantumTheme& theme);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumMixerSlider)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumMixerSliderWithLevelMeter : public NanoSubWidget,
                                         public IdleCallback
{
    const QuantumTheme& theme;

public:
    explicit QuantumMixerSliderWithLevelMeter(TopLevelWidget* parent, const QuantumTheme& theme);

    // publicly exposed for convenience, please do not resize or reposition these
    QuantumLevelMeter meter;
    QuantumMixerSlider slider;

protected:
    void onNanoDisplay() override;
    void onPositionChanged(const PositionChangedEvent& ev) override;
    void onResize(const ResizeEvent& ev) override;
    void idleCallback() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumMixerSliderWithLevelMeter)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueSlider : public NanoSubWidget,
                           public KnobEventHandler
{
    const QuantumTheme& theme;
    Color backgroundColor = theme.widgetDefaultActiveColor;
    char* unitLabel = nullptr;

public:
    explicit QuantumValueSlider(TopLevelWidget* parent, const QuantumTheme& theme);

    inline Color getBackgroundColor() const noexcept
    {
        return backgroundColor;
    }

    inline const char* getUnitLabel() const noexcept
    {
        return unitLabel;
    }

    void setBackgroundColor(Color color);
    void setUnitLabel(const char* label);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueSlider)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueMeter16 : public NanoSubWidget
{
    const QuantumTheme& theme;
    float values[16] = {};

public:
    explicit QuantumValueMeter16(TopLevelWidget* parent, const QuantumTheme& theme);

    void setValue(uint index, float value);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueMeter16)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumFrameGroup : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit QuantumFrameGroup(TopLevelWidget* parent, const QuantumTheme& theme);

protected:
    void onNanoDisplay() override;
    void onPositionChanged(const PositionChangedEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumFrameGroup)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumValueSliderGroup : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit QuantumValueSliderGroup(TopLevelWidget* parent, const QuantumTheme& theme);

    // publicly exposed for convenience, please do not resize or reposition these
    QuantumSwitch mainSwitch;
    // FIXME use proper children proceedure
    std::vector<QuantumValueSlider*> groupedWidgets;

protected:
    void onNanoDisplay() override;
    void onPositionChanged(const PositionChangedEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueSliderGroup)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
