// DarkBreeze widgets for DPF
// Copyright (C) 2025 Filipe Coelho <falktx@falktx.com>
// SPDX-License-Identifier: ISC

// NOTE this file is WORK-IN-PROGRESS

#include "DarkBreeze.hpp"

START_NAMESPACE_DGL

static constexpr const char kColorBackgroundActive[] = "#292c30";
static constexpr const char kColorBackgroundInactive[] = "#202326";
static constexpr const char kColorSeparatorLine[] = "#535659";
static constexpr const char kColorText[] = "#fcfcfc";

static constexpr const char kColorButtonFillActive[] = "#2d4655";
static constexpr const char kColorButtonFillActiveHover[] = "#30576e";
#define kColorButtonFillHover kColorBackgroundActive
#define kColorButtonFillDefault kColorBackgroundActive
static constexpr const char kColorButtonStrokeActive[] = "#3daee9";
#define kColorButtonStrokeActiveHover kColorButtonStrokeActive
#define kColorButtonStrokeHover kColorButtonStrokeActive
#define kColorButtonStrokeDefault kColorSeparatorLine

static constexpr const uint kSizePadding = 1;
static constexpr const uint kSizeButton = 32;
static constexpr const uint kSizeRadius = 4;

// --------------------------------------------------------------------------------------------------------------------

DarkBreeze::DarkBreeze(Window& window)
    : NanoTopLevelWidget(window),
      colorActive(Color::fromHTML(kColorBackgroundActive)),
      colorInactive(Color::fromHTML(kColorBackgroundInactive)) {}

void DarkBreeze::setActive(const bool active2)
{
    if (active == active2)
        return;

    active = active2;
    repaint();
}

void DarkBreeze::onNanoDisplay()
{
    beginPath();
    rect(0, 0, getWidth(), getHeight());
    fillColor(active ? colorActive : colorInactive);
    fill();
}

// --------------------------------------------------------------------------------------------------------------------

DarkBreezeButton::DarkBreezeButton(DarkBreeze* const parent)
    : NanoSubWidget(parent),
      ButtonEventHandler(this)
{
    loadSharedResources();

    constexpr uint size = kSizeButton + kSizePadding * 2;
    setSize(size, size);
}

DarkBreezeButton::~DarkBreezeButton()
{
    std::free(label);
}

void DarkBreezeButton::setLabel(const char* const label2)
{
    std::free(label);
    label = label2 != nullptr ? strdup(label2) : nullptr;

    if (label != nullptr && label[0] != '\0')
    {
        Rectangle<float> bounds;
        // fontSize(theme.fontSize);
        textBounds(0, 0, label, nullptr, bounds);

        const uint width = std::max(getWidth(), d_roundToUnsignedInt(bounds.getWidth()) + kSizePadding * 8);
        const uint height = std::max(getHeight(), d_roundToUnsignedInt(bounds.getHeight()) + kSizePadding * 2);
        setSize(width, height);
    }
}

void DarkBreezeButton::onNanoDisplay()
{
    beginPath();
    roundedRect(kSizePadding,
                kSizePadding,
                getWidth() - kSizePadding * 2,
                getHeight() - kSizePadding * 2,
                kSizeRadius);

    d_stdout("state %d", getState());
    switch (getState())
    {
    case kButtonStateActive:
        fillColor(Color::fromHTML(kColorButtonFillActive));
        strokeColor(Color::fromHTML(kColorButtonStrokeActive));
        break;
    case kButtonStateActiveHover:
        fillColor(Color::fromHTML(kColorButtonFillActiveHover));
        strokeColor(Color::fromHTML(kColorButtonStrokeActiveHover));
        break;
    case kButtonStateHover:
        fillColor(Color::fromHTML(kColorButtonFillHover));
        strokeColor(Color::fromHTML(kColorButtonStrokeHover));
        break;
    case kButtonStateDefault:
        fillColor(Color::fromHTML(kColorButtonFillDefault));
        strokeColor(Color::fromHTML(kColorButtonStrokeDefault));
        break;
    }

    fill();
    strokeWidth(1);
    stroke();

    if (label != nullptr && label[0] != '\0')
    {
        fillColor(Color::fromHTML(kColorText));
        // fontSize(theme.fontSize);
        textAlign(ALIGN_CENTER|ALIGN_MIDDLE);
        text(getWidth() / 2, getHeight() / 2, label, nullptr);
    }
}

bool DarkBreezeButton::onMouse(const MouseEvent& ev)
{
    return mouseEvent(ev);
}

bool DarkBreezeButton::onMotion(const MotionEvent& ev)
{
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
