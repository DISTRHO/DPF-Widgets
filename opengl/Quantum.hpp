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
    // background color for widgets, e.g. slider line and knob padding, typically dark
    Color widgetBackgroundColor = Color::fromHTML("#141414");
    // foreground color for widgets, e.g. slider handle or knob indicator, typically light
    Color widgetForegroundColor = Color::fromHTML("#8c8c8c");
    // border and line size for widgets, e.g. slider line and knob outer border
    int widgetBorderAndLineSize = 4;
    // window background, typically lighter than widget background
    Color windowBackgroundColor = Color::fromHTML("#3d3d3d");
    // text color, light / brightest tone
    Color textLightColor = Color::fromHTML("#b3b3b3");
    // text color, mid tone
    Color textMidColor = Color::fromHTML("#8c8c8c");
    // text color, dark tone
    Color textDarkColor = Color::fromHTML("#383838");
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumSwitch : public NanoSubWidget,
                      public ButtonEventHandler
{
    char* label = nullptr;
    const QuantumTheme& theme;

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
    char* labelLeft = nullptr;
    char* labelRight = nullptr;
    const QuantumTheme& theme;

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
    Color backgroundColor = Color::fromHTML("#3f535a");
    const QuantumTheme& theme;

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
    float value = 0.f;
    float falloff = 0.f;
    const QuantumTheme& theme;

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
    Color backgroundColor = Color::fromHTML("#3f535a");
    char* unitLabel = nullptr;
    const QuantumTheme& theme;

public:
    explicit QuantumValueSlider(TopLevelWidget* parent, const QuantumTheme& theme);

    Color getBackgroundColor() const noexcept;
    void setBackgroundColor(Color color);

    // returns null if label is empty
    const char* getUnitLabel() const noexcept;
    void setUnitLabel(const char* label);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;
    bool onScroll(const ScrollEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumValueSlider)
};

// --------------------------------------------------------------------------------------------------------------------

class QuantumGroup : public NanoSubWidget
{
    const QuantumTheme& theme;

public:
    explicit QuantumGroup(TopLevelWidget* parent, const QuantumTheme& theme);

    // publicly exposed for convenience, please do not resize or reposition these
    QuantumSwitch mainSwitch;
    // FIXME use proper children proceedure
    std::vector<NanoSubWidget*> groupedWidgets;

protected:
    void onNanoDisplay() override;
    void onPositionChanged(const PositionChangedEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuantumGroup)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
