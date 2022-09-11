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
#include "NanoVG.hpp"
#include "Application.hpp"
#include "DistrhoUtils.hpp"

#include <cmath>

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

QuantumButton::QuantumButton(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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

void QuantumButton::setBackgroundColor(const Color color)
{
    backgroundColor = color;
    repaint();
}

void QuantumButton::setLabel(const char* const label2, const bool adjustSizeNow)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;
    labelHasNewLine = label != nullptr && std::strchr(label, '\n') != nullptr;

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
        if (labelHasNewLine)
            textBox(0, (getHeight() - theme.fontSize) * 0.5f, getWidth(), label, nullptr);
        else
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

QuantumLabel::QuantumLabel(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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
        width = std::max(static_cast<uint>(bounds.getWidth() + 0.5f), theme.padding) + theme.textPixelRatioWidthCompensation;
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

void QuantumLabel::setLabelColor(const Color color)
{
    labelColor = color;
    repaint();
}

void QuantumLabel::onNanoDisplay()
{
    if (label == nullptr || label[0] == '\0')
        return;

    fillColor(labelColor);
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

template<bool horizontal>
AbstractQuantumSeparatorLine<horizontal>::AbstractQuantumSeparatorLine(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(horizontal ? QuantumMetrics(t).separatorHorizontal : QuantumMetrics(t).separatorVertical);
}

template<bool horizontal>
AbstractQuantumSeparatorLine<horizontal>::AbstractQuantumSeparatorLine(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    setSize(horizontal ? QuantumMetrics(t).separatorHorizontal : QuantumMetrics(t).separatorVertical);
}

template<bool horizontal>
void AbstractQuantumSeparatorLine<horizontal>::onNanoDisplay()
{
    beginPath();
    if (horizontal)
        rect(0, static_cast<int>(getHeight()/2), getWidth(), theme.borderSize);
    else
        rect(static_cast<int>(getWidth() / 2), 0, theme.borderSize, getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();
}

template class AbstractQuantumSeparatorLine<false>;
template class AbstractQuantumSeparatorLine<true>;

// --------------------------------------------------------------------------------------------------------------------

QuantumSpacer::QuantumSpacer(TopLevelWidget* const parent)
    : SubWidget(parent)
{
    setSkipDrawing();
}

QuantumSpacer::QuantumSpacer(SubWidget* const parent)
    : SubWidget(parent)
{
    setSkipDrawing();
}

void QuantumSpacer::onDisplay()
{
}

// --------------------------------------------------------------------------------------------------------------------

template<bool small>
AbstractQuantumSwitch<small>::AbstractQuantumSwitch(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(small ? QuantumMetrics(t).smallSwitch : QuantumMetrics(t).normalSwitch);
}

template<bool small>
AbstractQuantumSwitch<small>::AbstractQuantumSwitch(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      ButtonEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setCheckable(true);
    setSize(small ? QuantumMetrics(t).smallSwitch : QuantumMetrics(t).normalSwitch);
}

template<bool small>
AbstractQuantumSwitch<small>::~AbstractQuantumSwitch()
{
    std::free(label);
}

template<bool small>
void AbstractQuantumSwitch<small>::adjustSize()
{
    const uint blockSize = small ? theme.textHeight / 2 : theme.fontSize;

    uint width = blockSize * 2 + theme.borderSize * 2;
    uint height = theme.borderSize * 2;

    if (label != nullptr && label[0] != '\0')
    {
        Rectangle<float> bounds;
        fontSize(theme.fontSize);

        textBounds(0, 0, label, nullptr, bounds);
        width += theme.padding * 3 + static_cast<uint>(bounds.getWidth() + 0.5f);
        height += std::max(static_cast<uint>(bounds.getHeight() + 0.5f), blockSize);
    }
    else
    {
        height += blockSize;
    }

    setSize(width, height);
}

template<bool small>
void AbstractQuantumSwitch<small>::setLabel(const char* const label2, const bool adjustSizeNow)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (adjustSizeNow)
        adjustSize();
}

template<bool small>
void AbstractQuantumSwitch<small>::onNanoDisplay()
{
    const bool checked = isChecked();
    const uint blockSize = small ? theme.textHeight / 2 : theme.fontSize;
    const int yOffset = (getHeight() - blockSize - theme.borderSize * 2) / 2;

    beginPath();
    rect(0, yOffset, blockSize * 2 + theme.borderSize * 2, blockSize + theme.borderSize * 2);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    if (checked)
    {
        rect(theme.borderSize + blockSize , yOffset + theme.borderSize, blockSize, blockSize);
        fillColor(theme.widgetDefaultActiveColor);
    }
    else
    {
        rect(theme.borderSize, yOffset + theme.borderSize, blockSize, blockSize);
        fillColor(theme.textDarkColor);
    }
    fill();

    if (label != nullptr && label[0] != '\0')
    {
        fillColor(checked ? theme.textLightColor : theme.textMidColor);
        fontSize(theme.fontSize);
        textAlign(ALIGN_LEFT|ALIGN_MIDDLE);
        text(blockSize * 2 + theme.borderSize * 2 + theme.padding * 2, getHeight() / 2, label, nullptr);
    }
}

template<bool small>
bool AbstractQuantumSwitch<small>::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

template<bool small>
bool AbstractQuantumSwitch<small>::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

template class AbstractQuantumSwitch<false>;
template class AbstractQuantumSwitch<true>;

// --------------------------------------------------------------------------------------------------------------------

/*
QuantumDualSidedSwitch::QuantumDualSidedSwitch(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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
    fontSize(theme.fontSize);

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

    fontSize(theme.fontSize);

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

QuantumKnob::QuantumKnob(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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
    return mouseEvent(ev, getTopLevelWidget()->getScaleFactor());
}

bool QuantumKnob::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev, getTopLevelWidget()->getScaleFactor());
}

// --------------------------------------------------------------------------------------------------------------------

QuantumMixerSlider::QuantumMixerSlider(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setMouseDeceleration(500.f);
    setSize(QuantumMetrics(t).mixerSlider);
}

QuantumMixerSlider::QuantumMixerSlider(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setMouseDeceleration(500.f);
    setSize(QuantumMetrics(t).mixerSlider);
}

QuantumMixerSlider::~QuantumMixerSlider()
{
}

Rectangle<int> QuantumMixerSlider::getValueFieldAbsoluteArea() const noexcept
{
    const uint width = getWidth();
    const uint height = getHeight();

    const float sliderHandleHeight = theme.textHeight * 2;
    const float valueBoxStartY = height - sliderHandleHeight + theme.borderSize + theme.padding;

    return Rectangle<int>(getAbsoluteX(), getAbsoluteY() + valueBoxStartY, width, height - valueBoxStartY);
}

void QuantumMixerSlider::onNanoDisplay()
{
    const uint width = getWidth();
    const uint height = getHeight();

    // everything is relative to slider line
    const float sliderHandleWidth = theme.textHeight;
    const float sliderHandleHeight = theme.textHeight * 2;
    const float sliderLineHeightFor70dB = height - sliderHandleHeight;
    const float sliderLineHeightFor2dB = sliderLineHeightFor70dB * (1.f - normalizedLevelMeterValue(-2.f));
    const float sliderLineHeightFor48dB = sliderLineHeightFor70dB * (1.f - normalizedLevelMeterValue(-50.f)) - sliderLineHeightFor2dB;
    const float sliderLineStartX = width * 0.5f;
    const float sliderLineStartY0dB = theme.borderSize + sliderHandleHeight * 0.5f;
    const float sliderLineStartY2dB = sliderLineStartY0dB + sliderLineHeightFor2dB;
    const float valueBoxStartY = height - sliderHandleHeight + theme.borderSize + theme.padding;

    // bottom box and value
    beginPath();
    rect(0, valueBoxStartY, width, height - valueBoxStartY);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, valueBoxStartY + theme.borderSize, width - theme.borderSize * 2, height - valueBoxStartY - theme.borderSize * 2);
    fillColor(Color(theme.windowBackgroundColor, theme.widgetBackgroundColor, 0.75f));
    fill();

    char valuestr[32] = {};
    const int value = static_cast<int>(getValue());
    std::snprintf(valuestr, sizeof(valuestr)-1, "%d", value);

    fillColor(theme.textLightColor);
    textAlign(ALIGN_CENTER|ALIGN_BOTTOM);
    fontSize(theme.fontSize);
    text(width * 0.5f, height - theme.textHeight * 0.5f + theme.borderSize, valuestr, nullptr);

    // top label
    fontSize(theme.fontSize * 2 / 3);
    text(width * 0.5f, theme.textHeight, "Target", nullptr);

    // slider line
    strokeColor(Color(Color(), theme.widgetBackgroundColor, 0.5f));
    strokeWidth(theme.widgetLineSize);

    beginPath();
    moveTo(sliderLineStartX, sliderLineStartY2dB);
    lineTo(sliderLineStartX, sliderLineStartY2dB + sliderLineHeightFor48dB);
    stroke();

    beginPath();
    moveTo(sliderLineStartX - sliderHandleWidth / 2, sliderLineStartY2dB);
    lineTo(sliderLineStartX + sliderHandleWidth / 2, sliderLineStartY2dB);
    stroke();

    beginPath();
    moveTo(sliderLineStartX - sliderHandleWidth / 2, sliderLineStartY2dB + sliderLineHeightFor48dB);
    lineTo(sliderLineStartX + sliderHandleWidth / 2, sliderLineStartY2dB + sliderLineHeightFor48dB);
    stroke();

    strokeWidth(std::max(1.f, theme.widgetLineSize * 0.5f));

    for (int i=1; i<25; ++i)
    {
        const float tracesY = sliderLineStartY2dB + (i * sliderLineHeightFor48dB / 25);
        beginPath();
        moveTo(sliderLineStartX - sliderHandleWidth / 4, tracesY);
        lineTo(sliderLineStartX + sliderHandleWidth / 4, tracesY);
        stroke();
    }

    // slider handle
    save();
    translate(sliderLineStartX - sliderHandleWidth / 2, (1.f - normalizedLevelMeterValue(getValue())) * sliderLineHeightFor70dB);

    const float round = std::max(1.f, static_cast<float>(theme.widgetLineSize) * 3/2);

    beginPath();
    roundedRect(0, 0, sliderHandleWidth, sliderHandleHeight, round);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    roundedRect(theme.borderSize, theme.borderSize,
                sliderHandleWidth - theme.borderSize * 2,
                sliderHandleHeight - theme.borderSize * 2,
                round);
    fillColor(theme.widgetForegroundColor);
    fill();

    // lineCap(ROUND);
    strokeColor(theme.widgetBackgroundColor);
    strokeWidth(theme.borderSize);

    for (int i=0; i<4; ++i)
    {
        const float tracesY = sliderHandleHeight / 3 + (i * sliderHandleHeight / 8);
        beginPath();
        moveTo(sliderHandleWidth * 5 / 16, tracesY);
        lineTo(sliderHandleWidth * 11 / 16, tracesY);
        stroke();
    }

    restore();
}

bool QuantumMixerSlider::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev, getTopLevelWidget()->getScaleFactor());
}

bool QuantumMixerSlider::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev, getTopLevelWidget()->getScaleFactor());
}

// --------------------------------------------------------------------------------------------------------------------

QuantumGainReductionMeter::QuantumGainReductionMeter(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).gainReductionMeter);
}

void QuantumGainReductionMeter::setValue(const float value2)
{
    if (d_isEqual(value, value2))
        return;

    value = value2;
    repaint();
}

void QuantumGainReductionMeter::onNanoDisplay()
{
    const uint width = getWidth();
    const uint height = getHeight();

    const float verticalReservedHeight = theme.textHeight;
    const float usableMeterHeight = height - verticalReservedHeight * 3;
    const float usableInnerMeterHeight = usableMeterHeight - theme.borderSize * 2;
    const float valueBoxStartY = height - verticalReservedHeight * 2 + theme.borderSize + theme.padding;

    // normal widget background
    beginPath();
    rect(0, verticalReservedHeight, width, usableMeterHeight);
    fillColor(theme.widgetBackgroundColor);
    fill();

    // alternate background
    beginPath();
    rect(theme.borderSize, theme.borderSize + verticalReservedHeight,
         width - theme.borderSize * 2, usableInnerMeterHeight);
    fillColor(Color(theme.windowBackgroundColor, theme.widgetBackgroundColor, 0.75f));
    fill();

    // meter
    if (d_isNotZero(value))
    {
        beginPath();
        if (value < 0.f)
        {
            // const float normalizedValue = std::max(0.f, std::min(1.f, value / -50.f));
            const float normalizedValue = (1.f - normalizedLevelMeterValue(value)) * 1.08f;
            rect(theme.borderSize, theme.borderSize + verticalReservedHeight + usableInnerMeterHeight / 2,
                 width - theme.borderSize * 2, usableInnerMeterHeight / 2 * normalizedValue);
        }
        else
        {
            // const float normalizedValue = std::max(0.f, std::min(1.f, value / 50.f));
            const float normalizedValue = (1.f - normalizedLevelMeterValue(-value)) * 1.08f;
            rect(theme.borderSize, theme.borderSize + verticalReservedHeight + usableInnerMeterHeight / 2 * (1.f - normalizedValue),
                 width - theme.borderSize * 2, usableInnerMeterHeight / 2 * normalizedValue);
        }
        fillColor(theme.widgetDefaultAlternativeColor);
        fill();
    }

    // helper lines with labels
    constexpr const float db5 = (1.f - normalizedLevelMeterValue(-5)) * 1.08f;
    constexpr const float db10 = (1.f - normalizedLevelMeterValue(-10)) * 1.08f;
    constexpr const float db20 = (1.f - normalizedLevelMeterValue(-20)) * 1.08f;
    constexpr const float db30 = (1.f - normalizedLevelMeterValue(-30)) * 1.08f;
    constexpr const float db40 = (1.f - normalizedLevelMeterValue(-40)) * 1.08f;
    fillColor(theme.textLightColor);
    fontSize(theme.fontSize);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    const float centerX = width * 0.5f;
    const float yOffset = theme.borderSize + verticalReservedHeight + usableInnerMeterHeight / 2;
    text(centerX, yOffset, "-  0  -", nullptr);
    text(centerX, yOffset + usableInnerMeterHeight / 2 * db5, "-  5  -", nullptr);
    text(centerX, yOffset + usableInnerMeterHeight / 2 * db10, "- 10 -", nullptr);
    text(centerX, yOffset + usableInnerMeterHeight / 2 * db20, "- 20 -", nullptr);
    text(centerX, yOffset + usableInnerMeterHeight / 2 * db30, "- 30 -", nullptr);
    text(centerX, yOffset + usableInnerMeterHeight / 2 * db40, "- 40 -", nullptr);
    text(centerX, yOffset - usableInnerMeterHeight / 2 * db5, "-  5  -", nullptr);
    text(centerX, yOffset - usableInnerMeterHeight / 2 * db10, "- 10 -", nullptr);
    text(centerX, yOffset - usableInnerMeterHeight / 2 * db20, "- 20 -", nullptr);
    text(centerX, yOffset - usableInnerMeterHeight / 2 * db30, "- 30 -", nullptr);
    text(centerX, yOffset - usableInnerMeterHeight / 2 * db40, "- 40 -", nullptr);

    // bottom box and value
    beginPath();
    rect(0, valueBoxStartY, width, height - valueBoxStartY);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, valueBoxStartY + theme.borderSize, width - theme.borderSize * 2, height - valueBoxStartY - theme.borderSize * 2);
    fillColor(Color(theme.windowBackgroundColor, theme.widgetBackgroundColor, 0.75f));
    fill();

    char valuestr[32] = {};
    const float roundedValue = std::round(value * 10.f)/10.f;
    std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f", roundedValue);

    fillColor(theme.textLightColor);
    textAlign(ALIGN_CENTER|ALIGN_BOTTOM);
    fontSize(theme.fontSize);
    text(width * 0.5f, height - theme.textHeight * 0.5f + theme.borderSize, valuestr, nullptr);

    // top label
    fontSize(theme.fontSize * 2 /3);
    text(width * 0.5f, verticalReservedHeight, "Lvl Gain", nullptr);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumValueMeter::QuantumValueMeter(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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

QuantumValueMeter::~QuantumValueMeter()
{
    std::free(unitLabel);
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

void QuantumValueMeter::setTextColor(const Color color)
{
    textColor = color;
    repaint();
}

void QuantumValueMeter::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(theme.widgetBackgroundColor);
    fill();

    const float normalizedValue = (value - minimum) / (maximum - minimum);
    float nullValue = 0.f;

    switch (orientation)
    {
    case LeftToRight:
        // 0.f
        break;
    case RightToLeft:
    case BottomToTop:
    case TopToBottom:
        nullValue = 1.0f;
        break;
    case CenterToSides:
    case MiddleToEdges:
        nullValue = 0.5f;
        break;
    }

    if (d_isNotEqual(normalizedValue, nullValue))
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
            rect(theme.borderSize, theme.borderSize + (getHeight() - theme.borderSize * 2) * normalizedValue,
                 getWidth() - theme.borderSize * 2, (getHeight() - theme.borderSize * 2) * (1.f - normalizedValue));
            break;
        case CenterToSides:
            if (normalizedValue < 0.5f)
                rect(theme.borderSize + (getWidth() - theme.borderSize * 2) * normalizedValue, theme.borderSize,
                     (getWidth() - theme.borderSize * 2) * (0.5f - normalizedValue), getHeight() - theme.borderSize * 2);
            else
                rect(getWidth() * 0.5f, theme.borderSize,
                     (getWidth() - theme.borderSize * 2) * (normalizedValue - 0.5f), getHeight() - theme.borderSize * 2);
            break;
        case MiddleToEdges:
            if (normalizedValue < 0.5f)
                rect(theme.borderSize, theme.borderSize + (getHeight() - theme.borderSize * 2) * (1.f - normalizedValue),
                     (getWidth() - theme.borderSize * 2), (getHeight() - theme.borderSize * 2) * (normalizedValue - 0.5f));
            else
                rect(theme.borderSize, getHeight() * 0.5f,
                     (getWidth() - theme.borderSize * 2), (getHeight() - theme.borderSize * 2) * (0.5f - normalizedValue));
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
    case CenterToSides:
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
    fontSize(theme.fontSize);
    fillColor(textColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(getWidth()/2, getHeight()/2, valuestr, nullptr);
}

// --------------------------------------------------------------------------------------------------------------------

QuantumValueSlider::QuantumValueSlider(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setMouseDeceleration(500.f);
    setOrientation(Both);
    setSize(QuantumMetrics(t).valueSlider);
}

QuantumValueSlider::QuantumValueSlider(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      KnobEventHandler(this),
      theme(t)
{
    loadSharedResources();
    setMouseDeceleration(500.f);
    setOrientation(Both);
    setSize(QuantumMetrics(t).valueSlider);
}

QuantumValueSlider::~QuantumValueSlider()
{
    std::free(unitLabel);
}

void QuantumValueSlider::setBackgroundColor(Color color)
{
    backgroundColor = color;
    repaint();
}

void QuantumValueSlider::setTextColor(const Color color)
{
    textColor = color;
    repaint();
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

    if (isInteger())
    {
        const int value = static_cast<int>(getValue());

        if (unitLabel != nullptr)
            std::snprintf(valuestr, sizeof(valuestr)-1, "%d %s", value, unitLabel);
        else
            std::snprintf(valuestr, sizeof(valuestr)-1, "%d", value);
    }
    else
    {
        const float roundedValue = std::round(getValue() * 10.0f)/10.0f;

        if (unitLabel != nullptr)
            std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f %s", roundedValue, unitLabel);
        else
            std::snprintf(valuestr, sizeof(valuestr)-1, "%.1f", roundedValue);
    }

    beginPath();
    fontSize(theme.fontSize);
    fillColor(textColor);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    text(getWidth()/2, getHeight()/2, valuestr, nullptr);
}

bool QuantumValueSlider::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev, getTopLevelWidget()->getScaleFactor());
}

bool QuantumValueSlider::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev, getTopLevelWidget()->getScaleFactor());
}

// --------------------------------------------------------------------------------------------------------------------

QuantumLevelMeter::QuantumLevelMeter(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : QuantumValueMeter(parent, t)
{
    loadSharedResources();
    setBackgroundColor(theme.levelMeterColor);
    setOrientation(BottomToTop);
}

QuantumLevelMeter::QuantumLevelMeter(NanoSubWidget* const parent, const QuantumTheme& t)
    : QuantumValueMeter(parent, t)
{
    loadSharedResources();
    setBackgroundColor(theme.levelMeterColor);
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
}

// --------------------------------------------------------------------------------------------------------------------

QuantumStereoLevelMeterWithLUFS::QuantumStereoLevelMeterWithLUFS(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      app(parent->getApp()),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).stereoLevelMeterWithLufs);
    app.addIdleCallback(this);
}

QuantumStereoLevelMeterWithLUFS::QuantumStereoLevelMeterWithLUFS(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      app(parent->getApp()),
      theme(t)
{
    loadSharedResources();
    setSize(QuantumMetrics(t).stereoLevelMeterWithLufs);
    app.addIdleCallback(this);
}

void QuantumStereoLevelMeterWithLUFS::setRange(const float min, const float max)
{
    minimum = min;
    maximum = max;
    repaint();
}

void QuantumStereoLevelMeterWithLUFS::setValueL(const float value)
{
    if (value >= falloffL)
    {
        falloffL = value;
        lastTimeL = timeL = app.getTime();
    }

    if (d_isEqual(valueL, value))
        return;

    valueL = value;

    repaint();
}

void QuantumStereoLevelMeterWithLUFS::setValueR(const float value)
{
    if (value >= falloffR)
    {
        falloffR = value;
        lastTimeR = timeR = app.getTime();
    }

    if (d_isEqual(valueR, value))
        return;

    valueR = value;

    repaint();
}

void QuantumStereoLevelMeterWithLUFS::setValueLufs(const float value)
{
    if (d_isEqual(valueLufs, value))
        return;

    valueLufs = value;
    repaint();
}

void QuantumStereoLevelMeterWithLUFS::setValues(const float l, const float r, const float lufs)
{
    falloffL = valueL = l;
    falloffR = valueR = r;
    valueLufs = lufs;
    lastTimeL = timeL = lastTimeR = timeR = 0;
    repaint();
}

void QuantumStereoLevelMeterWithLUFS::onNanoDisplay()
{
    const float verticalReservedHeight = theme.textHeight;
    const float usableMeterHeight = getHeight() - verticalReservedHeight * 2;
    const float centerX = static_cast<float>(getWidth()) / 2;

    beginPath();
    rect(0, verticalReservedHeight, getWidth(), usableMeterHeight);
    fillColor(theme.widgetBackgroundColor);
    fill();

    float value;
    char valuestr[32] = {};

    const float meterChannelWidth = theme.textHeight - theme.borderSize * 2;
    const float meterChannelHeight = usableMeterHeight - theme.borderSize * 2;

    const float pxl = theme.borderSize;
    const float pxlufs = theme.borderSize * 5 + meterChannelWidth;
    const float pxr = theme.borderSize * 11 + meterChannelWidth * 3;

    // alternate background
    fillColor(Color(theme.windowBackgroundColor, theme.widgetBackgroundColor, 0.75f));

    beginPath();
    rect(pxl,
         theme.borderSize + verticalReservedHeight,
         meterChannelWidth, meterChannelHeight);
    fill();

    beginPath();
    rect(pxlufs,
         theme.borderSize + verticalReservedHeight,
         meterChannelWidth * 2 + theme.borderSize * 2, meterChannelHeight);
    fill();

    beginPath();
    rect(pxr,
         theme.borderSize + verticalReservedHeight,
         meterChannelWidth, meterChannelHeight);
    fill();

    // fake spacer
    fillColor(Color(theme.widgetBackgroundColor, theme.windowBackgroundColor, 0.5f));

    beginPath();
    rect(pxlufs - theme.borderSize * 3, verticalReservedHeight,
         theme.borderSize * 2, meterChannelHeight + theme.borderSize * 2);
    fill();

    beginPath();
    rect(pxr - theme.borderSize * 3, verticalReservedHeight,
         theme.borderSize * 2, meterChannelHeight + theme.borderSize * 2);
    fill();

    // left channel
    value = normalizedLevelMeterValue(valueL);

    if (d_isNotZero(value))
    {
        beginPath();
        rect(pxl,
             theme.borderSize + verticalReservedHeight + meterChannelHeight * (1.f - value),
             meterChannelWidth, meterChannelHeight * value);
        fillColor(theme.levelMeterColor);
        fill();

        std::snprintf(valuestr, sizeof(valuestr)-1, "%.0f", valueL);
    }
    else
    {
        std::strncpy(valuestr, "-inf", sizeof(valuestr)-1);
    }

    fillColor(theme.textLightColor);
    fontSize(theme.fontSize * 2 / 3);
    textAlign(ALIGN_CENTER|ALIGN_BOTTOM);
    text(pxl + meterChannelWidth / 2,
         verticalReservedHeight, valuestr, nullptr);

    if (d_isNotEqual(valueL, falloffL))
    {
        value = normalizedLevelMeterValue(falloffL);
        const float y = theme.borderSize + verticalReservedHeight + meterChannelHeight * (1.f - value);

        beginPath();
        moveTo(pxl, y);
        lineTo(pxl + meterChannelWidth, y);
        strokeColor(theme.levelMeterColor);
        strokeWidth(theme.borderSize);
        stroke();
    }

    // right channel
    value = normalizedLevelMeterValue(valueR);

    if (d_isNotZero(value))
    {
        beginPath();
        rect(pxr,
             theme.borderSize + verticalReservedHeight + meterChannelHeight * (1.f - value),
             meterChannelWidth, meterChannelHeight * value);
        fillColor(theme.levelMeterColor);
        fill();

        std::snprintf(valuestr, sizeof(valuestr)-1, "%.0f", valueR);
    }
    else
    {
        std::strncpy(valuestr, "-inf", sizeof(valuestr)-1);
    }

    fillColor(theme.textLightColor);
    fontSize(theme.fontSize * 2 / 3);
    textAlign(ALIGN_CENTER|ALIGN_BOTTOM);
    text(pxr + meterChannelWidth / 2,
         verticalReservedHeight, valuestr, nullptr);

    if (d_isNotEqual(valueR, falloffR))
    {
        value = normalizedLevelMeterValue(falloffR);
        const float y = theme.borderSize + verticalReservedHeight + meterChannelHeight * (1.f - value);

        beginPath();
        moveTo(pxr, y);
        lineTo(pxr + meterChannelWidth, y);
        strokeColor(theme.levelMeterColor);
        strokeWidth(theme.borderSize);
        stroke();
    }

    // lufs
    value = normalizedLevelMeterValue(valueLufs);

    if (d_isNotZero(value))
    {
        beginPath();
        rect(pxlufs,
             theme.borderSize + verticalReservedHeight + meterChannelHeight * (1.f - value),
             meterChannelWidth * 2 + theme.borderSize * 2,
             meterChannelHeight * value);
        fillColor(theme.levelMeterAlternativeColor);
        fill();

        std::snprintf(valuestr, sizeof(valuestr)-1, "LUFS: %.1f", valueLufs);
    }
    else
    {
        std::strncpy(valuestr, "LUFS: -inf", sizeof(valuestr)-1);
    }

    fillColor(theme.textLightColor);
    fontSize(theme.fontSize);
    textAlign(ALIGN_LEFT|ALIGN_BOTTOM);
    text(theme.borderSize + theme.padding, getHeight() - theme.borderSize, valuestr, nullptr);

    // helper lines with labels
    constexpr const float db2 = 1.f - normalizedLevelMeterValue(-2);
    constexpr const float db5 = 1.f - normalizedLevelMeterValue(-5);
    constexpr const float db10 = 1.f - normalizedLevelMeterValue(-10);
    constexpr const float db20 = 1.f - normalizedLevelMeterValue(-20);
    constexpr const float db30 = 1.f - normalizedLevelMeterValue(-30);
    constexpr const float db40 = 1.f - normalizedLevelMeterValue(-40);
    constexpr const float db50 = 1.f - normalizedLevelMeterValue(-50);
    fillColor(theme.textLightColor);
    fontSize(theme.fontSize);
    textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
    const float yOffset = theme.borderSize + verticalReservedHeight;
    text(centerX, yOffset + usableMeterHeight * db2, "-  2  -", nullptr);
    text(centerX, yOffset + usableMeterHeight * db5, "-  5  -", nullptr);
    text(centerX, yOffset + usableMeterHeight * db10, "- 10 -", nullptr);
    text(centerX, yOffset + usableMeterHeight * db20, "- 20 -", nullptr);
    text(centerX, yOffset + usableMeterHeight * db30, "- 30 -", nullptr);
    text(centerX, yOffset + usableMeterHeight * db40, "- 40 -", nullptr);
    text(centerX, yOffset + usableMeterHeight * db50, "- 50 -", nullptr);
}

void QuantumStereoLevelMeterWithLUFS::idleCallback()
{
    const double time = app.getTime(); // in seconds

    // TESTING
    DISTRHO_SAFE_ASSERT_RETURN(falloffL >= valueL,);
    DISTRHO_SAFE_ASSERT_RETURN(falloffR >= valueR,);

    constexpr const double secondsToWaitForFalloffStart = 2;
    constexpr const double falloffDbPerSecond = 8.6;

    if (d_isEqual(valueL, falloffL))
    {
        lastTimeL = timeL = time;
    }
    else
    {
        const double diffSinceValueSet = time - timeL;
        const double diffSinceLastIdle = time - lastTimeL;
        lastTimeL = time;

        if (diffSinceValueSet >= secondsToWaitForFalloffStart)
        {
            falloffL = std::max(valueL, static_cast<float>(falloffL - falloffDbPerSecond * diffSinceLastIdle));
            repaint();
        }
    }

    if (d_isEqual(valueR, falloffR))
    {
        lastTimeR = timeR = time;
    }
    else
    {
        const double diffSinceValueSet = time - timeR;
        const double diffSinceLastIdle = time - lastTimeR;
        lastTimeR = time;

        if (diffSinceValueSet >= secondsToWaitForFalloffStart)
        {
            falloffR = std::max(valueR, static_cast<float>(falloffR - falloffDbPerSecond * diffSinceLastIdle));
            repaint();
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------

static inline
void respositionChildren(const Widget::PositionChangedEvent& ev, std::list<SubWidget*> children)
{
    const int deltaX = ev.pos.getX() - ev.oldPos.getX();
    const int deltaY = ev.pos.getY() - ev.oldPos.getY();

    for (SubWidget* w : children)
        w->setAbsolutePos(w->getAbsoluteX() + deltaX, w->getAbsoluteY() + deltaY);
}

template<class tMainWidget>
AbstractQuantumFrame<tMainWidget>::AbstractQuantumFrame(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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
    rect(0, offset, getWidth(), getHeight() - offset);
    fillColor(theme.widgetBackgroundColor);
    fill();

    beginPath();
    rect(theme.borderSize, offset + theme.borderSize, getWidth() - theme.borderSize * 2, getHeight() - theme.borderSize * 2 - offset);
    fillColor(offset != 0 ? theme.windowBackgroundColor : Color(theme.widgetBackgroundColor, theme.windowBackgroundColor, 0.5f));
    fill();
}

template<class tMainWidget>
void AbstractQuantumFrame<tMainWidget>::onPositionChanged(const PositionChangedEvent& ev)
{
    respositionChildren(ev, getChildren());
}

template class AbstractQuantumFrame<char>;

// --------------------------------------------------------------------------------------------------------------------

template<>
AbstractQuantumFrame<QuantumLabel>::AbstractQuantumFrame(NanoTopLevelWidget* const parent, const QuantumTheme& t)
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
    offset = QuantumMetrics(theme).smallSwitch.getHeight() + theme.padding + theme.borderSize;
}

template<>
void AbstractQuantumFrame<QuantumLabel>::onPositionChanged(const PositionChangedEvent& ev)
{
    respositionChildren(ev, getChildren());
    mainWidget.setAbsolutePos(ev.pos.getX() + theme.borderSize, ev.pos.getY());
}

template class AbstractQuantumFrame<QuantumLabel>;

// --------------------------------------------------------------------------------------------------------------------

template<>
AbstractQuantumFrame<QuantumSmallSwitch>::AbstractQuantumFrame(NanoTopLevelWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(this, t)
{
    mainWidget.setAbsolutePos(theme.borderSize, 0);
    mainWidget.setName("+ switch");
    setSize(QuantumMetrics(t).frame);
}

template<>
AbstractQuantumFrame<QuantumSmallSwitch>::AbstractQuantumFrame(NanoSubWidget* const parent, const QuantumTheme& t)
    : NanoSubWidget(parent),
      theme(t),
      mainWidget(this, t)
{
    mainWidget.setAbsolutePos(theme.borderSize, 0);
    mainWidget.setName("+ switch");
    setSize(QuantumMetrics(t).frame);
}

template<>
void AbstractQuantumFrame<QuantumSmallSwitch>::adjustMainWidgetSize()
{
    mainWidget.adjustSize();
    offset = QuantumMetrics(theme).smallSwitch.getHeight() + theme.padding + theme.borderSize;
}

template<>
void AbstractQuantumFrame<QuantumSmallSwitch>::onPositionChanged(const PositionChangedEvent& ev)
{
    respositionChildren(ev, getChildren());
    mainWidget.setAbsolutePos(ev.pos.getX() + theme.borderSize, ev.pos.getY());
}

template class AbstractQuantumFrame<QuantumSmallSwitch>;

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
