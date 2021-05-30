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
      parent(p)
{
    parent->pData->widgets.push_back(this);
    setSkipDrawing(true);
}

BlendishIndividualWidgetBase::~BlendishIndividualWidgetBase()
{
    parent->pData->widgets.remove(this);
}

void BlendishIndividualWidgetBase::onDisplay()
{
}

// --------------------------------------------------------------------------------------------------------------------

BlendishLabel::BlendishLabel(BlendishSubWidget* const parent)
    : BlendishIndividualWidgetBase(parent),
      label(nullptr)
{
    setSize(1*scaleFactor, BND_WIDGET_HEIGHT*scaleFactor);
}

const char* BlendishLabel::getLabel() const
{
    return label;
}

void BlendishLabel::setLabel(const char* const label2)
{
    free(label);

    if (label2 != nullptr && label2[0] != '\0')
    {
        label = strdup(label2);
        setWidth(bndLabelWidth(context, 0, label2)*scaleFactor);
    }
    else
    {
        label = nullptr;
        setWidth(1*scaleFactor);
    }
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
      callback(),
      button(-1),
      state(BND_DEFAULT),
      label(nullptr)
{
    setSize(BND_TOOL_WIDTH*scaleFactor, BND_WIDGET_HEIGHT*scaleFactor);
}

const char* BlendishToolButton::getLabel() const
{
    return label;
}

void BlendishToolButton::setLabel(const char* const label2)
{
    free(label);

    if (label2 != nullptr && label2[0] != '\0')
    {
        label = strdup(label2);
        setWidth((BND_TOOL_WIDTH + bndLabelWidth(context, 0, label2))*scaleFactor);
    }
    else
    {
        label = nullptr;
        setWidth(BND_TOOL_WIDTH*scaleFactor);
    }
}

void BlendishToolButton::onBlendishDisplay()
{
    bndToolButton(context,
                  getAbsoluteX()/scaleFactor,
                  getAbsoluteY()/scaleFactor,
                  getWidth()/scaleFactor,
                  getHeight()/scaleFactor,
                  BND_CORNER_NONE, static_cast<BNDwidgetState>(state), 0, label);
}

bool BlendishToolButton::onMouse(const MouseEvent& ev)
{
    // button was released, handle it now
    if (button != -1 && ! ev.press)
    {
        DISTRHO_SAFE_ASSERT(state == BND_ACTIVE);

        // release button
        const int button2 = button;
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

        if (callback != nullptr)
            callback->blendishButtonClicked(this, button2);

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

bool BlendishToolButton::onMotion(const MotionEvent& ev)
{
    // keep pressed
    if (button != -1)
        return true;

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

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
