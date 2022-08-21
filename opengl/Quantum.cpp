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
#include "DistrhoUtils.hpp"
#include "NanoVG.hpp"
#include "SubWidget.hpp"

#include <cmath>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

QuantumButton::QuantumButton(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).button);
}

QuantumButton::QuantumButton(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).button);
}

QuantumButton::~QuantumButton()
{
    std::free(label);
}

void QuantumButton::adjustSize()
{
    uint width = theme.textHeight + theme.borderSize * 2;
    uint height = theme.borderSize * 2;

    if (label != nullptr && label[0] != '\0')
    {
        Rectangle<float> bounds;
        fontSize(theme.fontSize);

        textBounds(0, 0, label, nullptr, bounds);
        width += theme.padding * 2 + static_cast<uint>(bounds.getWidth() + 0.5f);
        height += std::max(static_cast<uint>(bounds.getHeight() + 0.5f), theme.textHeight);
    }
    else
    {
        height += theme.textHeight;
    }

    setSize(width, height);
}

void QuantumButton::setLabel(const char* const label2, const bool adjustSizeNow)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (adjustSizeNow)
        adjustSize();
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

    if (label != nullptr && label[0] != '\0')
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
    setSize(QuantumMetrics(t).label);
}

QuantumLabel::QuantumLabel(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).label);
}

QuantumLabel::~QuantumLabel()
{
    std::free(label);
}

void QuantumLabel::adjustSize()
{
    uint width, height;

    if (label != nullptr && label[0] != '\0')
    {
        Rectangle<float> bounds;
        fontSize(theme.fontSize);

        textBounds(0, 0, label, nullptr, bounds);
        width = std::max(static_cast<uint>(bounds.getWidth() + 0.5f), theme.padding);
        height = std::max(static_cast<uint>(bounds.getHeight() + 0.5f), theme.textHeight);
    }
    else
    {
        width = theme.padding;
        height = theme.textHeight;
    }

    setSize(width, height);
}

void QuantumLabel::setAlignment(const uint alignment2)
{
    if (alignment == alignment2)
        return;

    alignment = alignment2;
    repaint();
}

void QuantumLabel::setLabel(const char* const label2, const bool adjustSizeNow)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (adjustSizeNow)
        adjustSize();
}

void QuantumLabel::onNanoDisplay()
{
    if (label == nullptr || label[0] == '\0')
        return;

    fillColor(theme.textLightColor);
    fontSize(theme.fontSize);
    textAlign(alignment);

    float y;
    if (alignment & ALIGN_MIDDLE)
        y = getHeight() / 2;
    else if (alignment & ALIGN_BOTTOM)
        y = getHeight();
    else
        y = 0;

    textBox(0, y, getWidth(), label);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumSeparatorLine::QuantumSeparatorLine(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(QuantumMetrics(t).separator);
}

QuantumSeparatorLine::QuantumSeparatorLine(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(QuantumMetrics(t).separator);
}

void QuantumSeparatorLine::onNanoDisplay()
{
    beginPath();
    rect(0, static_cast<int>(getHeight()/2), getWidth(), theme.borderSize);
    fillColor(theme.widgetBackgroundColor);
    fill();
}

// --------------------------------------------------------------------------------------------------------------------

QuantumSpacer::QuantumSpacer(TopLevelWidget* const parent)
    : SubWidget(parent) {}

QuantumSpacer::QuantumSpacer(SubWidget* const parent)
    : SubWidget(parent) {}

void QuantumSpacer::onDisplay()
{
}

// --------------------------------------------------------------------------------------------------------------------

QuantumSwitch::QuantumSwitch(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(QuantumMetrics(t).switch_);
}

QuantumSwitch::QuantumSwitch(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(QuantumMetrics(t).switch_);
}

QuantumSwitch::~QuantumSwitch()
{
    std::free(label);
}

void QuantumSwitch::adjustSize()
{
    uint width = theme.textHeight + theme.borderSize * 2;
    uint height = theme.borderSize * 2;

    if (label != nullptr && label[0] != '\0')
    {
        Rectangle<float> bounds;
        fontSize(theme.fontSize);

        textBounds(0, 0, label, nullptr, bounds);
        width += theme.padding * 3 + static_cast<uint>(bounds.getWidth() + 0.5f);
        height += std::max(static_cast<uint>(bounds.getHeight() + 0.5f), theme.textHeight / 2);
    }
    else
    {
        height += theme.textHeight / 2;
    }

    setSize(width, height);
}

void QuantumSwitch::setLabel(const char* const label2, const bool adjustSizeNow)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (adjustSizeNow)
        adjustSize();
}

void QuantumSwitch::onNanoDisplay()
{
    const bool checked = isChecked();

    const int yOffset = (getHeight() - theme.textHeight / 2 - theme.borderSize * 2) / 2;

    beginPath();
    rect(0, yOffset, theme.textHeight + theme.borderSize * 2, theme.textHeight / 2 + theme.borderSize * 2);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    if (checked)
    {
        rect(theme.borderSize, yOffset + theme.borderSize, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(Color::fromHTML("#3f535a"));
    }
    else
    {
        rect(theme.borderSize + theme.textHeight / 2 , yOffset + theme.borderSize, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(theme.textDarkColor);
    }
    fill();

    if (label != nullptr && label[0] != '\0')
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

// --------------------------------------------------------------------------------------------------------------------

/*
QuantumDualSidedSwitch::QuantumDualSidedSwitch(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(QuantumMetrics(t).switch_);
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

    uint width = theme.textHeight + theme.borderSize * 2 + theme.padding * 4;

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
        text(centerX - theme.textHeight / 2 - theme.padding * 2, getHeight() / 2, labelLeft, nullptr);
    }

    if (labelRight != nullptr)
    {
        fillColor(checked ? Color(255, 255, 255) : theme.textMidColor);
        textAlign(ALIGN_LEFT|ALIGN_MIDDLE);
        text(centerX + theme.textHeight / 2 + theme.padding * 2, getHeight() / 2, labelRight, nullptr);
    }

    beginPath();
    rect(centerX - theme.textHeight / 2 - theme.padding, 0, 
         theme.textHeight + theme.padding * 2, getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    if (checked)
    {
        rect(centerX, theme.padding, theme.textHeight / 2, theme.textHeight / 2);
        fillColor(Color::fromHTML("#3f535a"));
    }
    else
    {
        rect(centerX - theme.textHeight / 2 , theme.padding, theme.textHeight / 2, theme.textHeight / 2);
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
*/

// --------------------------------------------------------------------------------------------------------------------

QuantumKnob::QuantumKnob(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(QuantumMetrics(t).knob);
}

QuantumKnob::QuantumKnob(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(QuantumMetrics(t).knob);
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
    const float indicatorLineSize = radius/2 + theme.widgetLineSize/2;
    const float indicatorThickness = theme.widgetLineSize;

    beginPath();
    circle(centerX, centerY, radius);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    arc(centerX, centerY, radius - theme.borderSize, degToRad(0.0f), degToRad(360.0f), CCW);
    fillColor(backgroundColor);
    fill();

    lineCap(ROUND);
    strokeWidth(theme.widgetLineSize);

    save();
    translate(centerX, centerY);
    rotate(degToRad(45.0f) + getNormalizedValue() * degToRad(270.0f));
    beginPath();
    roundedRect(-indicatorThickness/2, indicatorThickness/2, indicatorThickness, indicatorLineSize, theme.widgetLineSize/2);
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

QuantumMixerSlider::QuantumMixerSlider(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    setSize(30, 128);
}

QuantumMixerSlider::QuantumMixerSlider(NanoSubWidget* const parent, const QuantumTheme& t)
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

QuantumValueMeter::QuantumValueMeter(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).valueMeterHorizontal);
}

QuantumValueMeter::QuantumValueMeter(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).valueMeterHorizontal);
}

void QuantumValueMeter::setBackgroundColor(Color color)
{
    backgroundColor = color;
}

void QuantumValueMeter::setOrientation(const Orientation orientation2)
{
    orientation = orientation2;
}

void QuantumValueMeter::setRange(const float min, const float max)
{
    DISTRHO_SAFE_ASSERT_RETURN(max > min,);

    minimum = min;
    maximum = max;
}

void QuantumValueMeter::setUnitLabel(const char* const label)
{
    std::free(unitLabel);

    if (label != nullptr && label[0] != '\0')
        unitLabel = strdup(label);
    else
        unitLabel = nullptr;
}

void QuantumValueMeter::setValue(const float value2)
{
    if (d_isEqual(value, value2))
        return;

    value = value2;
    repaint();
}

void QuantumValueMeter::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    const float normalizedValue = (value - minimum) / (maximum - minimum);

    if (d_isNotZero(normalizedValue))
    {
        beginPath();
        switch (orientation)
        {
        case LeftToRight:
            rect(theme.borderSize, theme.borderSize, 
                 (getWidth() - theme.borderSize * 2) * normalizedValue, getHeight() - theme.borderSize * 2);
            break;
        case RightToLeft:
            rect(theme.borderSize + (getWidth() - theme.borderSize * 2) * normalizedValue, theme.borderSize,
                 (getWidth() - theme.borderSize * 2) * (1.f - normalizedValue), getHeight() - theme.borderSize * 2);
            break;
        case TopToBottom:
            rect(theme.borderSize, theme.borderSize,
                 getWidth() - theme.borderSize * 2, (getHeight() - theme.borderSize * 2) * (1.f - normalizedValue));
            break;
        case BottomToTop:
            rect(theme.borderSize, theme.borderSize + (getHeight() - theme.borderSize * 2) * (1.f - normalizedValue),
                 getWidth() - theme.borderSize * 2, (getHeight() - theme.borderSize * 2) * normalizedValue);
            break;
        }
        fillColor(backgroundColor);
        fill();
    }

    // only draw value text if in horizontal orientation
    switch (orientation)
    {
    case LeftToRight:
    case RightToLeft:
        break;
    default:
        return;
    }

    char valuestr[32] = {};
    const float roundedValue = std::round(value * 10.0f)/10.0f;

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

// --------------------------------------------------------------------------------------------------------------------

QuantumValueSlider::QuantumValueSlider(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setOrientation(Horizontal);
    setSize(QuantumMetrics(t).valueSlider);
}

QuantumValueSlider::QuantumValueSlider(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setOrientation(Horizontal);
    setSize(QuantumMetrics(t).valueSlider);
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

bool QuantumValueSlider::setValue(const float value, const bool sendCallback) noexcept
{
    if (KnobEventHandler::setValue(value, sendCallback))
    {
        repaint();
        return true;
    }

    return false;
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

QuantumLevelMeter::QuantumLevelMeter(TopLevelWidget* const parent, const QuantumTheme& t)
    : QuantumValueMeter(parent, t)
{
    loadSharedResources();
    setBackgroundColor(Color(93, 231, 61));
    setOrientation(BottomToTop);
}

QuantumLevelMeter::QuantumLevelMeter(NanoSubWidget* const parent, const QuantumTheme& t)
    : QuantumValueMeter(parent, t)
{
    loadSharedResources();
    setBackgroundColor(Color(93, 231, 61));
    setOrientation(BottomToTop);
}

void QuantumLevelMeter::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    const float normalizedValue = (value - minimum) / (maximum - minimum);

    if (d_isNotZero(normalizedValue))
    {
        beginPath();
        rect(theme.borderSize, theme.borderSize + (getHeight() - theme.borderSize * 2) * (1.f - normalizedValue),
             getWidth() - theme.borderSize * 2, (getHeight() - theme.borderSize * 2) * normalizedValue);
        fillColor(backgroundColor);
        fill();
    }

#if 0
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
#endif
}

void QuantumLevelMeter::idleCallback()
{
#if 0
    if (d_isEqual(value, falloff))
        return;

    // should not typically happen
    if (value > falloff)
    {
        falloff = value;
        return;
    }
#endif
}

// --------------------------------------------------------------------------------------------------------------------

QuantumValueMeter17::QuantumValueMeter17(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(32, 32);
}

void QuantumValueMeter17::setValue(const uint index, const float value)
{
    DISTRHO_SAFE_ASSERT_INT_RETURN(index < ARRAY_SIZE(values), index,);

    if (d_isEqual(values[index], value))
        return;

    values[index] = value;
    repaint();
}

void QuantumValueMeter17::onNanoDisplay()
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

template<class tMainWidget>
AbstractQuantumFrame<tMainWidget>::AbstractQuantumFrame(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(32, 32);
}

template<class tMainWidget>
AbstractQuantumFrame<tMainWidget>::AbstractQuantumFrame(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(32, 32);
}

template<class tMainWidget>
void AbstractQuantumFrame<tMainWidget>::adjustMainWidgetSize()
{
}

template<class tMainWidget>
void AbstractQuantumFrame<tMainWidget>::onNanoDisplay()
{
    beginPath();
    rect(0, offset, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, offset + theme.borderSize, getWidth() - theme.borderSize * 2, getHeight() - theme.borderSize * 2 - offset);
    fillColor(offset != 0 ? theme.windowBackgroundColor : Color(theme.widgetBackgroundColor, theme.windowBackgroundColor, 0.5f));
    fill();
}

template<class tMainWidget>
void AbstractQuantumFrame<tMainWidget>::onPositionChanged(const PositionChangedEvent&)
{
}

template class AbstractQuantumFrame<char>;

// --------------------------------------------------------------------------------------------------------------------

template<>
AbstractQuantumFrame<QuantumLabel>::AbstractQuantumFrame(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(this, t)
{
    mainWidget.setAbsolutePos(theme.borderSize, 0);
    mainWidget.setName("+ label");
    setSize(QuantumMetrics(t).frame);
}

template<>
AbstractQuantumFrame<QuantumLabel>::AbstractQuantumFrame(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(this, t)
{
    mainWidget.setAbsolutePos(theme.borderSize, 0);
    mainWidget.setName("+ label");
    setSize(QuantumMetrics(t).frame);
}

template<>
void AbstractQuantumFrame<QuantumLabel>::adjustMainWidgetSize()
{
    mainWidget.adjustSize();

    // keep switch vs label height consistent
    offset = QuantumMetrics(theme).switch_.getHeight() + theme.padding + theme.borderSize;
}

template<>
void AbstractQuantumFrame<QuantumLabel>::onPositionChanged(const PositionChangedEvent& ev)
{
    mainWidget.setAbsolutePos(ev.pos.getX() + theme.borderSize, ev.pos.getY());
}

template class AbstractQuantumFrame<QuantumLabel>;

// --------------------------------------------------------------------------------------------------------------------

template<>
AbstractQuantumFrame<QuantumSwitch>::AbstractQuantumFrame(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(this, t)
{
    mainWidget.setAbsolutePos(theme.borderSize, 0);
    mainWidget.setName("+ switch");
    setSize(QuantumMetrics(t).frame);
}

template<>
AbstractQuantumFrame<QuantumSwitch>::AbstractQuantumFrame(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(this, t)
{
    mainWidget.setAbsolutePos(theme.borderSize, 0);
    mainWidget.setName("+ switch");
    setSize(QuantumMetrics(t).frame);
}

template<>
void AbstractQuantumFrame<QuantumSwitch>::adjustMainWidgetSize()
{
    mainWidget.adjustSize();
    offset = QuantumMetrics(theme).switch_.getHeight() + theme.padding + theme.borderSize;
}

template<>
void AbstractQuantumFrame<QuantumSwitch>::onPositionChanged(const PositionChangedEvent& ev)
{
    mainWidget.setAbsolutePos(ev.pos.getX() + theme.borderSize, ev.pos.getY());
}

template class AbstractQuantumFrame<QuantumSwitch>;

// --------------------------------------------------------------------------------------------------------------------

/*
template<class tMainWidget>
QuantumGroupWithVerticallyStackedLayout<tMainWidget>::QuantumGroupWithVerticallyStackedLayout(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(parent, t)
{
    setSize(mainWidget.getWidth() + theme.borderSize * 2 + theme.padding * 3,
            mainWidget.getHeight() + theme.borderSize * 2 + theme.padding * 3);
}

template<>
QuantumGroupWithVerticallyStackedLayout<QuantumSwitch>::QuantumGroupWithVerticallyStackedLayout(TopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(parent, t)
{
    mainWidget.setCheckable(true);
    mainWidget.setChecked(true, false);

    setSize(mainWidget.getWidth() + theme.borderSize * 2 + theme.padding * 3,
            mainWidget.getHeight() + theme.borderSize * 2 + theme.padding * 3);
}

template<class tMainWidget>
void QuantumGroupWithVerticallyStackedLayout<tMainWidget>::adjustSize()
{
    const QuantumMetrics metrics(theme);

    uint width = 0;
    uint height = 0;

    for (HorizontalLayout* l : layout.items)
    {
        uint lineWidth = 0;

        for (SubWidgetWithSizeHint& s : l->widgets)
        {
            if (lineWidth != 0)
                lineWidth += theme.padding;

            lineWidth += s.widget->getWidth();
            height = std::max(height, s.widget->getHeight());
        }

        width = std::max(width, lineWidth);
    }

    if (height != 0)
    {
        height *= layout.items.size();
        height += theme.padding * (layout.items.size() - 1);
    }

    width = std::max(width, metrics.switch_.getWidth() + theme.borderSize * 2 + theme.padding * 3);
    height += metrics.switch_.getHeight() + theme.borderSize * 2 + theme.padding * 3;

    mainWidget.setSize(width, metrics.switch_.getHeight());
    setSize(width, height);
}

template<class tMainWidget>
void QuantumGroupWithVerticallyStackedLayout<tMainWidget>::showAll()
{
    show();
    mainWidget.show();

    for (HorizontalLayout* l : layout.items)
    {
        for (SubWidgetWithSizeHint& s : l->widgets)
            s.widget->show();
    }
}

template<class tMainWidget>
void QuantumGroupWithVerticallyStackedLayout<tMainWidget>::hideAll()
{
    hide();
    mainWidget.hide();

    for (HorizontalLayout* l : layout.items)
    {
        for (SubWidgetWithSizeHint& s : l->widgets)
            s.widget->hide();
    }
}

template<class tMainWidget>
void QuantumGroupWithVerticallyStackedLayout<tMainWidget>::onNanoDisplay()
{
    beginPath();
    rect(0, mainWidget.getHeight() + theme.padding, getWidth(), getHeight() - mainWidget.getHeight() - theme.padding);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, mainWidget.getHeight() + theme.padding + theme.borderSize,
         getWidth() - theme.borderSize * 2, getHeight() - mainWidget.getHeight() - theme.padding - theme.borderSize * 2);
    fillColor(theme.windowBackgroundColor);
    fill();
}

template<>
void QuantumGroupWithVerticallyStackedLayout<QuantumSwitch>::onNanoDisplay()
{
    beginPath();
    rect(0, mainWidget.getHeight() + theme.padding, getWidth(), getHeight() - mainWidget.getHeight() - theme.padding);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, mainWidget.getHeight() + theme.padding + theme.borderSize,
         getWidth() - theme.borderSize * 2, getHeight() - mainWidget.getHeight() - theme.padding - theme.borderSize * 2);
    fillColor(mainWidget.isChecked() ? theme.windowBackgroundColor : Color(theme.widgetBackgroundColor, theme.windowBackgroundColor, 0.75f));
    fill();
}

template<class tMainWidget>
void QuantumGroupWithVerticallyStackedLayout<tMainWidget>::onPositionChanged(const PositionChangedEvent& ev)
{
    mainWidget.setAbsolutePos(ev.pos.getX(), ev.pos.getY());
    layout.setAbsolutePos(ev.pos.getX() + theme.borderSize + theme.padding,
                          mainWidget.getAbsoluteY() + mainWidget.getHeight() + theme.borderSize + theme.padding * 2, theme.padding);
}

template<class tMainWidget>
void QuantumGroupWithVerticallyStackedLayout<tMainWidget>::onResize(const ResizeEvent& ev)
{
    layout.setWidth(ev.size.getWidth() - theme.borderSize * 2 - theme.padding * 2, theme.padding);
    NanoSubWidget::onResize(ev);
}

template class QuantumGroupWithVerticallyStackedLayout<QuantumLabel>;
template class QuantumGroupWithVerticallyStackedLayout<QuantumSwitch>;
*/

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
