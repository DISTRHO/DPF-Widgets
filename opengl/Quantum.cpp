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

#include "Quantum.hpp"

#include <cmath>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

QuantumButton::QuantumButton(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setSize(theme.textHeight + theme.borderSize * 2,
            theme.textHeight + theme.borderSize * 2);
}

QuantumButton::~QuantumButton()
{
    std::free(label);
}

void QuantumButton::setLabel(const char* const label2, const bool adjustWidth)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (!adjustWidth)
        return;

    uint width = theme.textHeight + theme.borderSize * 2 + theme.padding;

    if (label != nullptr)
    {
        width += theme.padding;

        Rectangle<float> bounds;
        fontSize(theme.fontSize);

        textBounds(0, 0, label, nullptr, bounds);
        width += bounds.getWidth();
    }

    setWidth(width);
}

void QuantumButton::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, theme.borderSize, getWidth() - theme.borderSize * 2, getHeight() - theme.borderSize * 2);

    if (isCheckable())
    {
        if (isChecked())
            fillColor(backgroundColor);
        else if (getState() == kButtonStateHover)
            fillColor(Color(backgroundColor, theme.widgetBackgroundColor, 0.75f));
    }
    else
    {
        switch (getState())
        {
        case kButtonStateActiveHover:
        case kButtonStateActive:
            fillColor(backgroundColor);
            break;
        case kButtonStateHover:
            fillColor(Color(backgroundColor, theme.widgetBackgroundColor, 0.75f));
            break;
        case kButtonStateDefault:
            fillColor(theme.widgetBackgroundColor);
            break;
        }
    }

    fill();

    if (label != nullptr)
    {
        fillColor(theme.textLightColor);
        fontSize(theme.fontSize);
        textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
        text(getWidth() / 2, getHeight() / 2, label, nullptr);
    }
}

bool QuantumButton::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantumButton::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumLabel::QuantumLabel(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    loadSharedResources();
    setSize(theme.textHeight + theme.padding * 2,
            theme.textHeight + theme.padding * 2);
}

QuantumLabel::~QuantumLabel()
{
    std::free(label);
}

void QuantumLabel::setLabel(const char* const label2, const bool adjustWidth)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (!adjustWidth)
        return;

    uint width = theme.textHeight + theme.padding * 2;

    if (label != nullptr)
    {
        Rectangle<float> bounds;
        fontSize(theme.fontSize);

        textBounds(0, 0, label, nullptr, bounds);
        width += bounds.getWidth();
    }

    setWidth(width);
}

void QuantumLabel::onNanoDisplay()
{
    if (label == nullptr)
        return;

    beginPath();
    rect(0, 0, getWidth(), getHeight());

    fillColor(theme.textLightColor);
    fontSize(theme.fontSize);
    textAlign(ALIGN_TOP|ALIGN_LEFT);
    textBox(theme.padding, theme.padding, getWidth() - theme.padding * 2, label);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumSwitch::QuantumSwitch(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(theme.textHeight * 2 + theme.borderSize * 2,
            theme.textHeight / 2 + theme.borderSize * 2);
}

QuantumSwitch::~QuantumSwitch()
{
    std::free(label);
}

void QuantumSwitch::setLabel(const char* const label2)
{
    std::free(label);
    label = strdup(label2);

    Rectangle<float> bounds;
    fontSize(16);

    uint width = theme.textHeight + theme.borderSize * 2 + theme.padding * 3;

    textBounds(0, 0, label2, nullptr, bounds);
    width += bounds.getWidth();

    setWidth(width);
}

void QuantumSwitch::onNanoDisplay()
{
    const bool checked = isChecked();

    beginPath();
    rect(0, 0, theme.textHeight + theme.borderSize * 2, getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    if (checked)
    {
        rect(theme.borderSize, theme.borderSize, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(Color::fromHTML("#3f535a"));
    }
    else
    {
        rect(theme.borderSize + theme.textHeight / 2 , theme.borderSize, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(theme.textDarkColor);
    }
    fill();

    if (label != nullptr)
    {
        fillColor(checked ? Color(255, 255, 255) : theme.textMidColor);
        fontSize(theme.fontSize);
        textAlign(ALIGN_LEFT|ALIGN_MIDDLE);
        text(theme.textHeight + theme.borderSize * 2 + theme.padding * 2, getHeight() / 2, label, nullptr);
    }
}

bool QuantumSwitch::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantumSwitch::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

#if 0
// --------------------------------------------------------------------------------------------------------------------

QuantumDualSidedSwitch::QuantumDualSidedSwitch(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(theme.textHeight * 2 + theme.widgetBorderAndLineSize * 2,
            theme.textHeight / 2 + theme.widgetBorderAndLineSize * 2);
}

QuantumDualSidedSwitch::~QuantumDualSidedSwitch()
{
    std::free(labelLeft);
    std::free(labelRight);
}

void QuantumDualSidedSwitch::setLabels(const char* const left, const char* const right)
{
    std::free(labelLeft);
    std::free(labelRight);
    labelLeft = strdup(left);
    labelRight = strdup(right);

    Rectangle<float> bounds;
    fontSize(16);

    uint width = theme.textHeight + theme.widgetBorderAndLineSize * 8;

    textBounds(0, 0, labelLeft, nullptr, bounds);
    width += bounds.getWidth();

    textBounds(0, 0, labelRight, nullptr, bounds);
    width += bounds.getWidth();

    setWidth(width);
}

void QuantumDualSidedSwitch::onNanoDisplay()
{
    const float centerX = getWidth() / 2;
    const bool checked = isChecked();

    fontSize(16);

    if (labelLeft != nullptr)
    {
        fillColor(checked ? theme.textMidColor : Color(255, 255, 255));
        textAlign(ALIGN_RIGHT|ALIGN_MIDDLE);
        text(centerX - theme.textHeight / 2 - theme.widgetBorderAndLineSize * 2, getHeight() / 2, labelLeft, nullptr);
    }

    if (labelRight != nullptr)
    {
        fillColor(checked ? Color(255, 255, 255) : theme.textMidColor);
        textAlign(ALIGN_LEFT|ALIGN_MIDDLE);
        text(centerX + theme.textHeight / 2 + theme.widgetBorderAndLineSize * 2, getHeight() / 2, labelRight, nullptr);
    }

    beginPath();
    rect(centerX - theme.textHeight / 2 - theme.widgetBorderAndLineSize, 0, 
         theme.textHeight + theme.widgetBorderAndLineSize * 2, getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    if (checked)
    {
        rect(centerX, theme.widgetBorderAndLineSize, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(Color::fromHTML("#3f535a"));
    }
    else
    {
        rect(centerX - theme.textHeight / 2 , theme.widgetBorderAndLineSize, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(Color::fromHTML("#585338"));
    }
    fill();
}

bool QuantumDualSidedSwitch::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantumDualSidedSwitch::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumKnob::QuantumKnob(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(36, 36);
}

Color QuantumKnob::getBackgroundColor() const noexcept
{
    return backgroundColor;
}

void QuantumKnob::setBackgroundColor(Color color)
{
    backgroundColor = color;
}

void QuantumKnob::onNanoDisplay()
{
    const float centerX = getWidth() / 2;
    const float centerY = getHeight() / 2;
    const float radius = std::min(centerX, centerY);
    const float indicatorLineSize = radius/2 + theme.widgetBorderAndLineSize/2;
    const float indicatorThickness = theme.widgetBorderAndLineSize;

    beginPath();
    circle(centerX, centerY, radius);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    arc(centerX, centerY, radius - theme.widgetBorderAndLineSize, degToRad(0.0f), degToRad(360.0f), CCW);
    fillColor(backgroundColor);
    fill();

    lineCap(ROUND);
    strokeWidth(theme.widgetBorderAndLineSize);

    save();
    translate(centerX, centerY);
    rotate(degToRad(45.0f) + getNormalizedValue() * degToRad(270.0f));
    beginPath();
    roundedRect(-indicatorThickness/2, indicatorThickness/2, indicatorThickness, indicatorLineSize, theme.widgetBorderAndLineSize/2);
    closePath();
    fillColor(theme.widgetForegroundColor);
    fill();
    restore();
}

bool QuantumKnob::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantumKnob::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

bool QuantumKnob::onScroll(const ScrollEvent& ev)
{
    return scrollEvent(ev);
}

#endif

// --------------------------------------------------------------------------------------------------------------------

QuantumLevelMeter::QuantumLevelMeter(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    loadSharedResources();
    setSize(40, 128);
}

bool QuantumLevelMeter::setNormalizedValue(float newValue)
{
    if (d_isEqual(value, newValue))
        return true;

    value = newValue;

    if (newValue > falloff)
        falloff = newValue;

    repaint();
    return true;
}

void QuantumLevelMeter::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    const float usableWidth = getWidth() - theme.borderSize * 2;
    const float usableHeight = getHeight() - theme.borderSize * 2;

    // FIXME have linear value and do log scale on UI side
    const float valuableHeight = usableHeight * std::min(1.f, std::max(0.f, value > -90.f ? std::pow(10.f, value * 0.05f) : 0.f));

    beginPath();
    rect(theme.borderSize, theme.borderSize + usableHeight - valuableHeight, usableWidth, valuableHeight);
    fillColor(Color(93, 231, 61));
    fill();

    const float centerX = getWidth() / 2;
    char valuestr[32] = {};

    fontSize(16);
    textAlign(ALIGN_CENTER|ALIGN_TOP);

    // clipping
    if (value > 0.f)
    {
        beginPath();
        rect(theme.borderSize, theme.borderSize, usableWidth, theme.textHeight);
        fillColor(Color(200, 0, 0));
        fill();

        fillColor(theme.textLightColor);

         // level value
        std::snprintf(valuestr, sizeof(valuestr)-1, "+%.1f", value);
    }
    else
    {
        fillColor(theme.textMidColor);

        if (value < 120.f)
            std::strncpy(valuestr, "-inf", sizeof(valuestr)-1);
        else
            std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f", value);
    }

    text(centerX, theme.borderSize * 2, valuestr, nullptr);

    fillColor(theme.textDarkColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(centerX, theme.borderSize + usableHeight * 1/6, "-6-", nullptr);
    text(centerX, theme.borderSize + usableHeight * 2/6, "-18-", nullptr);
    text(centerX, theme.borderSize + usableHeight * 3/6, "-30-", nullptr);
    text(centerX, theme.borderSize + usableHeight * 4/6, "-42-", nullptr);
    text(centerX, theme.borderSize + usableHeight * 5/6, "-54-", nullptr);
}

void QuantumLevelMeter::idleCallback()
{
    if (d_isEqual(value, falloff))
        return;

    // should not typically happen
    if (value > falloff)
    {
        falloff = value;
        return;
    }
}

// --------------------------------------------------------------------------------------------------------------------

QuantumMixerSlider::QuantumMixerSlider(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(30, 128);
}

void QuantumMixerSlider::onNanoDisplay()
{
    const uint width = getWidth();

    // everything is relative to slider line
    const float sliderHandleHeight = width * 3 / 2;
    const float sliderLineHeight = getHeight() - sliderHandleHeight / 2;
    const float sliderLineX = width / 2;
    const float sliderLineY = sliderHandleHeight / 2;

    // slider line
    beginPath();
    // FIXME full size line??
    // moveTo(sliderLineX, sliderLineY);
    // lineTo(sliderLineX, sliderLineHeight);
    moveTo(sliderLineX, 0);
    lineTo(sliderLineX, getHeight());
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(theme.widgetLineSize);
    stroke();

    /*
    beginPath();
    moveTo(0, sliderLineY + sliderLineHeight * 0.15);
    lineTo(width, sliderLineY + sliderLineHeight * 0.15);
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(std::max(1, theme.widgetBorderAndLineSize / 2));
    stroke();
    */

    // slider handle
    save();
    translate(0, (1.0f - getNormalizedValue()) * (sliderLineHeight - sliderLineY));

    beginPath();
    roundedRect(0, 0, width, sliderHandleHeight, theme.borderSize*2);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    roundedRect(theme.borderSize, theme.borderSize,
                width - theme.borderSize * 2,
                sliderHandleHeight - theme.borderSize * 2,
                theme.borderSize * 3/2);
    fillColor(theme.widgetForegroundColor);
    fill();

    lineCap(ROUND);
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(theme.borderSize/2);

    for (int i=0; i<4; ++i)
    {
        const float tracesY = sliderHandleHeight / 3 + (i * sliderHandleHeight / 8);
        beginPath();
        moveTo(width * 5 / 16, tracesY);
        lineTo(width * 11 / 16, tracesY);
        stroke();
    }

    restore();
}

bool QuantumMixerSlider::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantumMixerSlider::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

bool QuantumMixerSlider::onScroll(const ScrollEvent& ev)
{
    return scrollEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumMixerSliderWithLevelMeter::QuantumMixerSliderWithLevelMeter(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      meter(parent, t),
      slider(parent, t)
{
    loadSharedResources();

    meter.setName("+ Level Meter");
    slider.setName("+ Mixer Slider");

    const uint meteringHeight = 300;

    meter.setAbsoluteX(theme.borderSize);
    meter.setHeight(meteringHeight);

    slider.setAbsoluteX(getWidth() - slider.getWidth() - theme.borderSize);
    slider.setHeight(meteringHeight);

    setSize(meter.getWidth() + slider.getWidth() + theme.borderSize * 3,
            meteringHeight + theme.textHeight + theme.borderSize);
}

void QuantumMixerSliderWithLevelMeter::onNanoDisplay()
{
    /*
    beginPath();
    rect(0, getHeight() - theme.textHeight, getWidth(), theme.textHeight);
    fillColor(theme.widgetBackgroundColor);
    fill();

    char valuestr[32] = {};
    std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f dB", slider.getValue());

    fontSize(16);
    fillColor(theme.textLightColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(getWidth()/2, getHeight() - theme.textHeight/2, valuestr, nullptr);
    */
}

void QuantumMixerSliderWithLevelMeter::onPositionChanged(const PositionChangedEvent& ev)
{
    const int x = getAbsoluteX();
    const int y = getAbsoluteY();
    meter.setAbsolutePos(x + theme.borderSize, y);
    slider.setAbsolutePos(x + getWidth() - slider.getWidth() - theme.borderSize, y);
}

void QuantumMixerSliderWithLevelMeter::onResize(const ResizeEvent& ev)
{
    // const uint width  = ev.size.getWidth();
    // const uint height = ev.size.getHeight();
}

void QuantumMixerSliderWithLevelMeter::idleCallback()
{
}

// --------------------------------------------------------------------------------------------------------------------

QuantumValueSlider::QuantumValueSlider(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setOrientation(Horizontal);
    setSize(72, theme.textHeight + theme.padding * 2);
}

void QuantumValueSlider::setBackgroundColor(Color color)
{
    backgroundColor = color;
}

void QuantumValueSlider::setUnitLabel(const char* const label)
{
    std::free(unitLabel);

    if (label != nullptr && label[0] != '\0')
        unitLabel = strdup(label);
    else
        unitLabel = nullptr;
}

void QuantumValueSlider::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    if (const float normalizedValue = getNormalizedValue())
    {
        beginPath();
        rect(theme.borderSize,
             theme.borderSize,
             (getWidth()-theme.borderSize*2) * normalizedValue,
             getHeight()-theme.borderSize*2);
        fillColor(backgroundColor);
        fill();
    }

    char valuestr[32] = {};
    const float roundedValue = std::round(getValue() * 10.0f)/10.0f;

    if (unitLabel != nullptr)
        std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f %s", roundedValue, unitLabel);
    else
        std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f", roundedValue);

    beginPath();
    fontSize(16);
    fillColor(theme.textLightColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(getWidth()/2, getHeight()/2, valuestr, nullptr);
}

bool QuantumValueSlider::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantumValueSlider::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

bool QuantumValueSlider::onScroll(const ScrollEvent& ev)
{
    return scrollEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumValueMeter16::QuantumValueMeter16(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(32, 32);
}

void QuantumValueMeter16::setValue(const uint index, const float value)
{
    DISTRHO_SAFE_ASSERT_INT_RETURN(index < ARRAY_SIZE(values), index,);

    if (d_isEqual(values[index], value))
        return;

    values[index] = value;
    repaint();
}

void QuantumValueMeter16::onNanoDisplay()
{
    /*
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();
    */

    const uint wpb = getWidth() / ARRAY_SIZE(values);

    fillColor(Color(93, 231, 61, 0.1f));
    strokeColor(theme.widgetBackgroundColor.withAlpha(0.25f));
    strokeWidth(theme.borderSize / 2);

    for (size_t i=0; i<ARRAY_SIZE(values); ++i)
    {
        const float usableHeight = getHeight() - theme.padding;
        const float valuableHeight = usableHeight * std::min(1.f, std::max(0.f, values[i] > -90.f ? std::pow(10.f, values[i] * 0.05f) : 0.f));

        beginPath();
        rect(wpb * i, theme.padding / 2 + usableHeight - valuableHeight,
             wpb, valuableHeight);
        fill();
        stroke();
    }
}

// --------------------------------------------------------------------------------------------------------------------

QuantumFrameGroup::QuantumFrameGroup(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(32, 32);
}

void QuantumFrameGroup::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.padding, theme.padding, getWidth() - theme.padding * 2, getHeight() - theme.padding * 2);
    fillColor(Color(theme.widgetBackgroundColor, theme.windowBackgroundColor, 0.5f));
    fill();
}

void QuantumFrameGroup::onPositionChanged(const PositionChangedEvent& ev)
{
    const int diffX = ev.pos.getX() - ev.oldPos.getX();
    const int diffY = ev.pos.getY() - ev.oldPos.getY();

    for (SubWidget* w : getChildren())
        w->setAbsolutePos(w->getAbsoluteX() + diffX, w->getAbsoluteY() + diffY);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumValueSliderGroup::QuantumValueSliderGroup(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainSwitch(parent, t)
{
    mainSwitch.setAbsolutePos(theme.padding, theme.padding);
    mainSwitch.setCheckable(true);
    mainSwitch.setChecked(true, false);

    setSize(mainSwitch.getWidth() + theme.padding * 2,
            mainSwitch.getHeight() + theme.padding * 2);
}

void QuantumValueSliderGroup::onNanoDisplay()
{
    beginPath();
    rect(0, mainSwitch.getHeight() + theme.padding * 2, getWidth(), getHeight() - mainSwitch.getHeight() - theme.padding);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.padding, mainSwitch.getHeight() + theme.padding * 3,
         getWidth() - theme.padding * 2, getHeight() - mainSwitch.getHeight() - theme.padding * 4);
    fillColor(theme.windowBackgroundColor);
    fill();
}

void QuantumValueSliderGroup::onPositionChanged(const PositionChangedEvent& ev)
{
    const int diffX = ev.pos.getX() - ev.oldPos.getX();
    const int diffY = ev.pos.getY() - ev.oldPos.getY();

    mainSwitch.setAbsolutePos(mainSwitch.getAbsoluteX() + diffX, mainSwitch.getAbsoluteY() + diffY);

    for (NanoSubWidget* w : groupedWidgets)
         w->setAbsolutePos(w->getAbsoluteX() + diffX, w->getAbsoluteY() + diffY);
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
