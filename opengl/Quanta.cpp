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

#include "Quanta.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

QuantaKnob::QuantaKnob(TopLevelWidget* const parent, const QuantaTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(36, 36);
}

Color QuantaKnob::getBackgroundColor() const noexcept
{
    return backgroundColor;
}

void QuantaKnob::setBackgroundColor(Color color)
{
    backgroundColor = color;
}

void QuantaKnob::onNanoDisplay()
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

bool QuantaKnob::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantaKnob::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

bool QuantaKnob::onScroll(const ScrollEvent& ev)
{
    return scrollEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

QuantaMixerSlider::QuantaMixerSlider(TopLevelWidget* const parent, const QuantaTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(36, 128);
}

void QuantaMixerSlider::onNanoDisplay()
{
    const uint width = getWidth();

    // everything is relative to slider line
    const float sliderHandleHeight = width * 3 / 2;
    const float sliderLineHeight = getHeight() - sliderHandleHeight / 2;
    const float sliderLineX = width / 2;
    const float sliderLineY = sliderHandleHeight / 2;

    // slider line
    beginPath();
    moveTo(sliderLineX, sliderLineY);
    lineTo(sliderLineX, sliderLineHeight);
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(theme.widgetBorderAndLineSize);
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

bool QuantaMixerSlider::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantaMixerSlider::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

bool QuantaMixerSlider::onScroll(const ScrollEvent& ev)
{
    return scrollEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

QuantaValueSlider::QuantaValueSlider(TopLevelWidget* const parent, const QuantaTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setOrientation(Horizontal);
    setSize(72, 36);
}

Color QuantaValueSlider::getBackgroundColor() const noexcept
{
    return backgroundColor;
}

void QuantaValueSlider::setBackgroundColor(Color color)
{
    backgroundColor = color;
}

const char* QuantaValueSlider::getUnitLabel() const noexcept
{
    return unitLabel;
}

void QuantaValueSlider::setUnitLabel(const char* const label)
{
    std::free(unitLabel);

    if (label != nullptr && label[0] != '\0')
        unitLabel = strdup(label);
    else
        unitLabel = nullptr;
}

void QuantaValueSlider::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    if (const float normalizedValue = getNormalizedValue())
    {
        beginPath();
        rect(theme.widgetBorderAndLineSize, theme.widgetBorderAndLineSize,
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

bool QuantaValueSlider::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool QuantaValueSlider::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

bool QuantaValueSlider::onScroll(const ScrollEvent& ev)
{
    return scrollEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
