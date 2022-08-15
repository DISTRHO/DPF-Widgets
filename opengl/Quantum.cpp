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

START_NAMESPACE_DGL

static constexpr const uint labelHeight = 24;

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

    const float usableWidth = getWidth() - theme.widgetBorderAndLineSize * 2;
    const float usableHeight = getHeight() - theme.widgetBorderAndLineSize * 2;
    const float valuableHeight = usableHeight * std::min(1.f, value);

    beginPath();
    rect(theme.widgetBorderAndLineSize, theme.widgetBorderAndLineSize + usableHeight - valuableHeight, usableWidth, valuableHeight);
    fillColor(Color(93, 231, 61));
    fill();

    const float centerX = getWidth() / 2;
    char valuestr[32] = {};

    fontSize(16);
    textAlign(ALIGN_CENTER|ALIGN_TOP);

    // clipping
    if (value > 1.f)
    {
        beginPath();
        rect(theme.widgetBorderAndLineSize, theme.widgetBorderAndLineSize, usableWidth, labelHeight);
        fillColor(Color(200, 0, 0));
        fill();

        fillColor(theme.textLightColor);

         // level value
        std::snprintf(valuestr, sizeof(valuestr)-1, "+%.1f", value);
    }
    else
    {
        fillColor(theme.textMidColor);

        if (d_isZero(value))
            std::strncpy(valuestr, "-inf", sizeof(valuestr)-1);
        else
            std::snprintf(valuestr, sizeof(valuestr)-1, "-%.1f", value);
    }

    text(centerX, theme.widgetBorderAndLineSize * 2, valuestr, nullptr);

    fillColor(theme.textDarkColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(centerX, theme.widgetBorderAndLineSize + usableHeight * 1/6, "-6-", nullptr);
    text(centerX, theme.widgetBorderAndLineSize + usableHeight * 2/6, "-18-", nullptr);
    text(centerX, theme.widgetBorderAndLineSize + usableHeight * 3/6, "-30-", nullptr);
    text(centerX, theme.widgetBorderAndLineSize + usableHeight * 4/6, "-42-", nullptr);
    text(centerX, theme.widgetBorderAndLineSize + usableHeight * 5/6, "-54-", nullptr);
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
    strokeWidth(theme.widgetBorderAndLineSize);
    stroke();

    beginPath();
    moveTo(0, sliderLineY + sliderLineHeight * 0.15);
    lineTo(width, sliderLineY + sliderLineHeight * 0.15);
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(std::max(1, theme.widgetBorderAndLineSize / 2));
    stroke();

    // slider handle
    save();
    translate(0, (1.0f - getNormalizedValue()) * (sliderLineHeight - sliderLineY));

    beginPath();
    roundedRect(0, 0, width, sliderHandleHeight, theme.widgetBorderAndLineSize*2);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    roundedRect(theme.widgetBorderAndLineSize, theme.widgetBorderAndLineSize,
                width - theme.widgetBorderAndLineSize * 2,
                sliderHandleHeight - theme.widgetBorderAndLineSize * 2,
                theme.widgetBorderAndLineSize * 3/2);
    fillColor(theme.widgetForegroundColor);
    fill();

    lineCap(ROUND);
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(theme.widgetBorderAndLineSize/2);

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

    const uint meteringHeight = 300;

    meter.setAbsoluteX(theme.widgetBorderAndLineSize);
    meter.setHeight(meteringHeight);

    slider.setAbsoluteX(getWidth() - slider.getWidth() - theme.widgetBorderAndLineSize);
    slider.setHeight(meteringHeight);

    setSize(meter.getWidth() + slider.getWidth() + theme.widgetBorderAndLineSize * 3,
            meteringHeight + labelHeight + theme.widgetBorderAndLineSize * 2);
}

void QuantumMixerSliderWithLevelMeter::onNanoDisplay()
{
    beginPath();
    rect(0, getHeight() - labelHeight, getWidth(), labelHeight);
    fillColor(theme.widgetBackgroundColor);
    fill();

    char valuestr[32] = {};
    std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f", slider.getNormalizedValue());

    fontSize(16);
    fillColor(theme.textLightColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(getWidth()/2, getHeight() - labelHeight/2, valuestr, nullptr);
}

void QuantumMixerSliderWithLevelMeter::onPositionChanged(const PositionChangedEvent& ev)
{
    const int x = getAbsoluteX();
    const int y = getAbsoluteY();
    meter.setAbsolutePos(x + theme.widgetBorderAndLineSize, y);
    slider.setAbsolutePos(x + getWidth() - slider.getWidth() - theme.widgetBorderAndLineSize, y);
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
    setSize(72, 36);
}

Color QuantumValueSlider::getBackgroundColor() const noexcept
{
    return backgroundColor;
}

void QuantumValueSlider::setBackgroundColor(Color color)
{
    backgroundColor = color;
}

const char* QuantumValueSlider::getUnitLabel() const noexcept
{
    return unitLabel;
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
        rect(theme.widgetBorderAndLineSize,
             theme.widgetBorderAndLineSize,
            (getWidth()-theme.widgetBorderAndLineSize*2) * normalizedValue,
            getHeight()-theme.widgetBorderAndLineSize*2);
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

END_NAMESPACE_DGL
