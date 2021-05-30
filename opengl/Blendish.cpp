/*
 * Blendish widgets for DPF
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
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

#include "Blendish.hpp"
#include "NanoVG.hpp"

// import DPF nanovg copy
#include "src/nanovg/nanovg.h"

// now we can use blendish
#define BLENDISH_IMPLEMENTATION
#include "Blendish/blendish.h"

#include <list>
#include <vector>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

struct BlendishSubWidget::PrivateData {
    NanoVG nvg;
    double scaleFactor;
    std::list<BlendishIndividualWidgetBase*> widgets;

    PrivateData()
    {
        nvg.loadSharedResources();
        bndSetFont(0);
        // TESTING
        bndSetIconImage(nvgCreateImage(nvg.getContext(), "./blender_icons16.png", 0));
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishSubWidget::BlendishSubWidget(Widget* const parent)
    : SubWidget(parent),
      pData(new PrivateData())
{
    pData->scaleFactor = getWindow().getScaleFactor();
    setNeedsViewportScaling(true, pData->scaleFactor);
}

BlendishSubWidget::~BlendishSubWidget()
{
    delete pData;
}

void BlendishSubWidget::onDisplay()
{
    pData->nvg.beginFrame(getWidth(), getHeight(), pData->scaleFactor);

    for (std::list<BlendishIndividualWidgetBase*>::iterator it = pData->widgets.begin(); it != pData->widgets.end(); ++it)
    {
        BlendishIndividualWidgetBase* const widget(*it);

        if (widget->isVisible())
            widget->onBlendishDisplay();
    }

    pData->nvg.endFrame();
}

// --------------------------------------------------------------------------------------------------------------------

BlendishIndividualWidgetBase::BlendishIndividualWidgetBase(BlendishSubWidget* const p)
    : SubWidget(p),
      context(p->pData->nvg.getContext()),
      scaleFactor(p->pData->scaleFactor),
      button(-1),
      flags(kCornerNone),
      state(BND_DEFAULT),
      label(nullptr),
      parent(p)
{
    parent->pData->widgets.push_back(this);
    setSkipDrawing(true);
}

BlendishIndividualWidgetBase::~BlendishIndividualWidgetBase()
{
    parent->pData->widgets.remove(this);
}

int BlendishIndividualWidgetBase::getCornerFlags() const noexcept
{
    return flags;
}

void BlendishIndividualWidgetBase::setCornerFlags(const int flags2)
{
    flags = flags2;
}

const char* BlendishIndividualWidgetBase::getLabel() const noexcept
{
    return label;
}

void BlendishIndividualWidgetBase::setLabel(const char* const label2)
{
    free(label);

    if (label2 != nullptr && label2[0] != '\0')
    {
        label = strdup(label2);
        setWidth((getMinimumWidth() + bndLabelWidth(context, 0, label2))*scaleFactor);
    }
    else
    {
        label = nullptr;
        setWidth(1*scaleFactor);
    }
}

bool BlendishIndividualWidgetBase::onMouse(const MouseEvent& ev)
{
    // button was released, handle it now
    if (button != -1 && ! ev.press)
    {
        DISTRHO_SAFE_ASSERT(state == BND_ACTIVE);

        // release button
        // const int button2 = button;
        button = -1;

        // cursor was moved outside the button bounds, ignore click
        if (! contains(ev.pos))
        {
            state = BND_DEFAULT;
            repaint();
            return true;
        }

        // still on bounds, register click
        state = BND_HOVER;
        repaint();

        // TODO callback

        return true;
    }

    // button was pressed, wait for release
    if (ev.press && contains(ev.pos))
    {
        button = static_cast<int>(ev.button);
        state  = BND_ACTIVE;
        repaint();
        return true;
    }

    return false;
}

bool BlendishIndividualWidgetBase::onMotion(const MotionEvent& ev)
{
    // keep pressed
    if (button != -1)
        return true;

    if (state == BND_ACTIVE)
    {
        // TODO change value
    }

    if (contains(ev.pos))
    {
        // check if entering hover
        if (state == BND_DEFAULT)
        {
            state = BND_HOVER;
            repaint();
            return true;
        }
    }
    else
    {
        // check if exiting hover
        if (state == BND_HOVER)
        {
            state = BND_DEFAULT;
            repaint();
            return true;
        }
    }

    return false;
}

void BlendishIndividualWidgetBase::onDisplay()
{
}

// --------------------------------------------------------------------------------------------------------------------

BlendishLabel::BlendishLabel(BlendishSubWidget* const parent)
    : BlendishIndividualWidgetBase(parent)
{
    setSize(1*scaleFactor, BND_WIDGET_HEIGHT*scaleFactor);
}

uint BlendishLabel::getMinimumWidth() const noexcept
{
    return 1;
}

void BlendishLabel::onBlendishDisplay()
{
    if (label == nullptr)
        return;

    bndLabel(context,
             getAbsoluteX()/scaleFactor,
             getAbsoluteY()/scaleFactor,
             getWidth()/scaleFactor,
             getHeight()/scaleFactor,
             0, label);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishToolButton::BlendishToolButton(BlendishSubWidget* const parent)
    : BlendishIndividualWidgetBase(parent),
      callback()
{
    setSize(BND_TOOL_WIDTH*scaleFactor, BND_WIDGET_HEIGHT*scaleFactor);
}

uint BlendishToolButton::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishToolButton::onBlendishDisplay()
{
    bndToolButton(context,
                  getAbsoluteX()/scaleFactor,
                  getAbsoluteY()/scaleFactor,
                  getWidth()/scaleFactor,
                  getHeight()/scaleFactor,
                  static_cast<BNDcornerFlags>(flags),
                  static_cast<BNDwidgetState>(state), 0, label);
}

//         if (callback != nullptr)
//             callback->blendishButtonClicked(this, button2);

// --------------------------------------------------------------------------------------------------------------------

BlendishNumberField::BlendishNumberField(BlendishSubWidget* const parent)
    : BlendishIndividualWidgetBase(parent),
      value(0)
{
    setSize(48*scaleFactor, BND_WIDGET_HEIGHT*scaleFactor);
}

uint BlendishNumberField::getMinimumWidth() const noexcept
{
    return 48;
}

int BlendishNumberField::getValue() const noexcept
{
    return value;
}

void BlendishNumberField::onBlendishDisplay()
{
    char valuestr[32];
    snprintf(valuestr, sizeof(valuestr)-1, "%d", value);

    bndNumberField(context,
                   getAbsoluteX()/scaleFactor,
                   getAbsoluteY()/scaleFactor,
                   getWidth()/scaleFactor,
                   getHeight()/scaleFactor,
                   static_cast<BNDcornerFlags>(flags),
                   static_cast<BNDwidgetState>(state), label, valuestr);
}

// --------------------------------------------------------------------------------------------------------------------
// simple function to group things together, setting corner flags automatically, must be null terminated

uint groupBlendishWidgets(const bool horizontal, BlendishIndividualWidgetBase* widget...)
{
    BlendishIndividualWidgetBase* prev = nullptr;
    uint numWidgetsSoFar = 0;
    uint pos = horizontal ? widget->getAbsoluteX() : widget->getAbsoluteY();

    // for vertical stacking
    uint widthSoFar = widget->getWidth();
    std::vector<BlendishIndividualWidgetBase*> widgets;

    va_list args;
    va_start(args, widget);

    do
    {
        if (horizontal)
        {
            if (prev != nullptr)
            {
                prev->setCornerFlags(prev->getCornerFlags() | BlendishIndividualWidgetBase::kCornerRight);
                widget->setAbsoluteX(pos - numWidgetsSoFar);
                widget->setCornerFlags(BlendishIndividualWidgetBase::kCornerLeft);
            }

            pos += widget->getWidth();
        }
        else
        {
            const uint width = widget->getWidth();

            if (width > widthSoFar)
            {
                widthSoFar = width;
                for (BlendishIndividualWidgetBase* w : widgets)
                    w->setWidth(width);
            }

            if (prev != nullptr)
            {
                prev->setCornerFlags(prev->getCornerFlags() | BlendishIndividualWidgetBase::kCornerDown);
                widget->setAbsoluteY(pos - numWidgetsSoFar);
                widget->setCornerFlags(BlendishIndividualWidgetBase::kCornerTop);
            }

            pos += widget->getHeight();
            widgets.push_back(widget);
        }

        prev = widget;
        ++numWidgetsSoFar;
    }
    while ((widget = va_arg(args, BlendishIndividualWidgetBase*)) != nullptr);

    va_end(args);

    return pos;
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
