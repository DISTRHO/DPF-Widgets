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

struct BlendishSubWidgetSharedContext::PrivateData {
    NanoVG nvg;
    double scaleFactor;
    std::list<BlendishSubWidget*> widgets;

    PrivateData()
        : nvg(),
          scaleFactor(1.0),
          widgets()
    {
        nvg.loadSharedResources();
        bndSetFont(0);
        // TESTING
        bndSetIconImage(nvgCreateImage(nvg.getContext(), "./blender_icons16.png", 0));
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishSubWidgetSharedContext::BlendishSubWidgetSharedContext(Widget* const parent)
    : SubWidget(parent),
      pData(new PrivateData())
{
    pData->scaleFactor = getWindow().getScaleFactor();
    setNeedsViewportScaling(true, pData->scaleFactor);
}

BlendishSubWidgetSharedContext::~BlendishSubWidgetSharedContext()
{
    delete pData;
}

void BlendishSubWidgetSharedContext::setScaleFactor(const double scaleFactor)
{
    pData->scaleFactor = scaleFactor;
    setNeedsViewportScaling(true, scaleFactor);
}

void BlendishSubWidgetSharedContext::onDisplay()
{
    pData->nvg.beginFrame(getWidth(), getHeight(), pData->scaleFactor);

    for (std::list<BlendishSubWidget*>::iterator it = pData->widgets.begin(); it != pData->widgets.end(); ++it)
    {
        BlendishSubWidget* const widget(*it);

        if (widget->isVisible())
            widget->onBlendishDisplay();
    }

    pData->nvg.endFrame();
}

// --------------------------------------------------------------------------------------------------------------------

struct BlendishSubWidget::ProtectedData : public ButtonEventHandler,
                                          public ButtonEventHandler::Callback  {
    BlendishSubWidget* const self;
    BlendishSubWidgetSharedContext* const shared;
    NanoVG* const nvg;
    NVGcontext* const context;
    double scaleFactor;

    int flags;
    bool labelCanChangeWidth;
    char* label;

    BlendishSubWidget::Callback* internalCallback;
    BlendishSubWidget::Callback* userCallback;

    Point<double> oldMotionPos;

    explicit ProtectedData(BlendishSubWidget* const s, BlendishSubWidgetSharedContext* const p)
        : ButtonEventHandler(s),
          self(s),
          shared(p),
          nvg(nullptr),
          context(p->pData->nvg.getContext()),
          scaleFactor(p->pData->scaleFactor),
          flags(kCornerNone),
          labelCanChangeWidth(true),
          label(nullptr),
          internalCallback(nullptr),
          userCallback(nullptr),
          oldMotionPos(0, 0)
    {
        ButtonEventHandler::setCallback(this);
        shared->pData->widgets.push_back(self);
    }

    explicit ProtectedData(BlendishSubWidget* const s, BlendishSubWidget* const parent)
        : ButtonEventHandler(s),
          self(s),
          shared(parent != nullptr ? parent->pData->shared : nullptr),
          nvg(parent != nullptr ? nullptr : new NanoVG()),
          context(parent != nullptr ? parent->pData->context : nvg->getContext()),
          scaleFactor(parent != nullptr ? parent->pData->scaleFactor : s->getWindow().getScaleFactor()),
          flags(kCornerNone),
          labelCanChangeWidth(true),
          label(nullptr),
          internalCallback(nullptr),
          userCallback(nullptr),
          oldMotionPos(0, 0)
    {
        ButtonEventHandler::setCallback(this);
    }

    ~ProtectedData()
    {
        if (shared != nullptr)
            shared->pData->widgets.remove(self);

        delete internalCallback;
        delete nvg;
    }

    BNDwidgetState getBlendishState() const noexcept
    {
        switch (getState())
        {
        case kButtonStateDefault:
            return BND_DEFAULT;
        case kButtonStateHover:
            return BND_HOVER;
        case kButtonStateActive:
        case kButtonStateActiveHover:
            return BND_ACTIVE;
        }

        return BND_DEFAULT;
    }

    void buttonClicked(SubWidget* widget, int button) override
    {
        BlendishSubWidget* const imageButton = dynamic_cast<BlendishSubWidget*>(widget);
        DISTRHO_SAFE_ASSERT_RETURN(imageButton != nullptr,);

        if (internalCallback != nullptr)
            internalCallback->blendishWidgetClicked(self, button);
        else if (userCallback != nullptr)
            userCallback->blendishWidgetClicked(self, button);
    }

    void stateChanged(const State state, const State oldState) override
    {
        const bool  isHovering = state    & kButtonStateHover;
        const bool wasHovering = oldState & kButtonStateHover;

        if (isHovering && ! wasHovering)
            if (BlendishMenu* const menu = dynamic_cast<BlendishMenu*>(self))
                if (menu->matchingComboBox != nullptr)
                    menu->matchingComboBox->pData->clearState();
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishSubWidget::BlendishSubWidget(BlendishSubWidgetSharedContext* const parent)
    : SubWidget(parent),
      pData(new ProtectedData(this, parent))
{
    setSkipDrawing(true);
}

BlendishSubWidget::BlendishSubWidget(SubWidget* const parent)
    : SubWidget(parent),
      pData(new ProtectedData(this, dynamic_cast<BlendishSubWidget*>(parent)))
{
    if (pData->shared == nullptr)
        setNeedsViewportScaling(true, pData->scaleFactor);
}

BlendishSubWidget::~BlendishSubWidget()
{
    delete pData;
}

int BlendishSubWidget::getCornerFlags() const noexcept
{
    return pData->flags;
}

void BlendishSubWidget::setCornerFlags(const int flags)
{
    pData->flags = flags;
}

const char* BlendishSubWidget::getLabel() const noexcept
{
    return pData->label;
}

void BlendishSubWidget::setLabel(const char* const label)
{
    std::free(pData->label);

    if (label != nullptr && label[0] != '\0')
    {
        pData->label = strdup(label);

        if (pData->labelCanChangeWidth)
            setWidth((getMinimumWidth() + bndLabelWidth(pData->context, 0, label)) * pData->scaleFactor);
    }
    else
    {
        pData->label = nullptr;

        if (pData->labelCanChangeWidth)
            setWidth(1*pData->scaleFactor);
    }
}

void BlendishSubWidget::toFront()
{
    if (pData->shared != nullptr)
    {
        std::list<BlendishSubWidget*>& widgets(pData->shared->pData->widgets);

        widgets.remove(this);
        widgets.push_back(this);
    }

    SubWidget::toFront();
}

BlendishSubWidgetSharedContext* BlendishSubWidget::getSharedContext()
{
    return pData->shared;
}

void BlendishSubWidget::setCallback(Callback* const callback)
{
    pData->userCallback = callback;
}

bool BlendishSubWidget::onMouse(const MouseEvent& ev)
{
    if (SubWidget::onMouse(ev))
        return true;
    if (pData->mouseEvent(ev))
        return true;
    return false;
}

bool BlendishSubWidget::onMotion(const MotionEvent& ev)
{
    if (SubWidget::onMotion(ev))
        return true;
    if (pData->motionEvent(ev))
        return true;
    return false;
}

void BlendishSubWidget::onDisplay()
{
    if (NanoVG* const nvg = pData->nvg)
    {
        nvg->beginFrame(getWidth(), getHeight(), pData->scaleFactor);
        onBlendishDisplay();
        nvg->endFrame();
    }
}

// --------------------------------------------------------------------------------------------------------------------

BlendishLabel::BlendishLabel(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent)
{
    setSize(1*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishLabel::BlendishLabel(SubWidget* const parent)
    : BlendishSubWidget(parent)
{
    setSize(1*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

uint BlendishLabel::getMinimumWidth() const noexcept
{
    return 1;
}

void BlendishLabel::onBlendishDisplay()
{
    if (pData->label == nullptr)
        return;

    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndLabel(pData->context, x, y, w, h, -1, pData->label);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishToolButton::BlendishToolButton(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishToolButton::BlendishToolButton(SubWidget* const parent)
    : BlendishSubWidget(parent)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

uint BlendishToolButton::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishToolButton::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndToolButton(pData->context, x, y, w, h,
                  static_cast<BNDcornerFlags>(pData->flags),
                  pData->getBlendishState(), -1, pData->label);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishCheckBox::BlendishCheckBox(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent)
{
    pData->setCheckable(true);
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishCheckBox::BlendishCheckBox(SubWidget* const parent)
    : BlendishSubWidget(parent)
{
    pData->setCheckable(true);
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

bool BlendishCheckBox::isChecked() const noexcept
{
    return pData->isChecked();
}

void BlendishCheckBox::setChecked(const bool checked)
{
    pData->setChecked(checked, false);
}

uint BlendishCheckBox::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishCheckBox::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    BNDwidgetState state;

    if (pData->isChecked())
    {
        state = BND_ACTIVE;
    }
    else
    {
        switch (pData->getBlendishState())
        {
        case BND_HOVER:
            state = BND_HOVER;
            break;
        default:
            state = BND_DEFAULT;
            break;
        }
    }

    bndOptionButton(pData->context, x, y, w, h, state, pData->label);
}

// --------------------------------------------------------------------------------------------------------------------

struct BlendishMenuItem::CallbackComboBox : BlendishSubWidget::Callback
{
    BlendishComboBox* const comboBox;
    BlendishMenu& menu;

    explicit CallbackComboBox(BlendishComboBox* const cb, BlendishMenu& m)
        : comboBox(cb),
          menu(m) {}

    void blendishWidgetClicked(BlendishSubWidget* const widget, const int button) override
    {
        if (button != 1)
            return;

        menu.hide();
        comboBox->setCurrentMenuItem(dynamic_cast<BlendishMenuItem*>(widget), true);

        if (widget->pData->userCallback != nullptr)
            widget->pData->userCallback->blendishWidgetClicked(widget, button);
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishMenuItem::BlendishMenuItem(BlendishMenu* const parent, const char* const label)
    : BlendishSubWidget(parent)
{
    setMargin(0, static_cast<int>(0.5 * pData->scaleFactor + 0.5));
    setSize(BND_TOOL_WIDTH * pData->scaleFactor, (BND_WIDGET_HEIGHT - 1) * pData->scaleFactor);

    if (label != nullptr)
        setLabel(label);
}

uint BlendishMenuItem::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishMenuItem::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndMenuItem(pData->context, x, y, w, h, pData->flags,
                pData->getBlendishState(), -1, pData->label);
}

// --------------------------------------------------------------------------------------------------------------------

struct BlendishMenu::CallbackComboBox : BlendishSubWidget::Callback
{
    BlendishMenu& menu;

    explicit CallbackComboBox(BlendishMenu& m)
        : menu(m) {}

    void blendishWidgetClicked(BlendishSubWidget* const widget, const int button) override
    {
        if (button != 1)
            return;

        if (menu.isVisible())
        {
            menu.hide();
            return;
        }

        menu.setAbsolutePos(widget->getAbsoluteX(), widget->getAbsoluteY() + widget->getHeight());
        menu.setWidth(std::max(widget->getWidth(), menu.biggestItemWidth));
        menu.toFront();
        menu.show();
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishMenu::BlendishMenu(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      items(),
      lastHoveredItem(nullptr),
      matchingComboBox(nullptr),
      biggestItemWidth(0),
      nextY(0),
      labelAtBottom(false)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishMenu::BlendishMenu(BlendishComboBox* const parent)
    : BlendishSubWidget(parent->getSharedContext()),
      items(),
      lastHoveredItem(nullptr),
      matchingComboBox(parent),
      biggestItemWidth(0),
      nextY(0),
      labelAtBottom(false)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishMenu::BlendishMenu(SubWidget* const parent)
    : BlendishSubWidget(parent),
      items(),
      lastHoveredItem(nullptr),
      matchingComboBox(nullptr),
      biggestItemWidth(0),
      nextY(0),
      labelAtBottom(false)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishMenuItem* BlendishMenu::addMenuItem(const char* const label)
{
    DISTRHO_SAFE_ASSERT_RETURN(label != nullptr && label[0] != '\0', nullptr);

    BlendishMenuItem* const item = new BlendishMenuItem(this, label);

    if (items.size() != 0)
    {
        BlendishMenuItem* const lastitem = items.back();
        lastitem->setCornerFlags(lastitem->getCornerFlags() | kCornerDown);
        item->setCornerFlags(kCornerTop);
    }
    else if (getLabel() != nullptr)
    {
        item->setCornerFlags(kCornerTop);
    }

    item->setAbsoluteX(getAbsoluteX());
    item->setAbsoluteY(nextY);
    items.push_back(item);

    nextY += item->getHeight();
    recheckSize(item->getWidth(), item->getHeight());

    return item;
}

void BlendishMenu::setLabel(const char* const label)
{
    BlendishSubWidget::setLabel(label);

    if (getLabel() != nullptr)
    {
        if (items.size() != 0)
        {
            BlendishMenuItem* const firstitem = items.front();
            firstitem->setCornerFlags(firstitem->getCornerFlags() | kCornerTop);
        }
    }
    else
    {
        if (items.size() != 0)
        {
            BlendishMenuItem* const firstitem = items.front();
            firstitem->setCornerFlags(firstitem->getCornerFlags() & ~kCornerTop);
        }
    }

    recheckSize(0, 0);
}

uint BlendishMenu::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishMenu::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndMenuBackground(pData->context, x, y, w, h, pData->flags);

    if (pData->label != nullptr)
        bndMenuLabel(pData->context,
                     x, labelAtBottom ? (h - BND_WIDGET_HEIGHT - 2) : y,
                     w, BND_WIDGET_HEIGHT, -1, pData->label);

    for (std::vector<BlendishMenuItem*>::iterator it = items.begin(); it != items.end(); ++it)
    {
        BlendishMenuItem* const item(*it);

        if (item->isVisible())
            item->onBlendishDisplay();
    }
}

bool BlendishMenu::onMotion(const MotionEvent& ev)
{
    const bool containsOldPos = contains(pData->oldMotionPos);

    if (BlendishSubWidget::onMotion(ev))
        return true;

    if (contains(ev.pos) && containsOldPos)
        return true;

    return false;
}

void BlendishMenu::onPositionChanged(const PositionChangedEvent& ev)
{
    const int x = ev.pos.getX();
    /* */ int y = ev.pos.getY();

    if (getLabel() != nullptr)
        y += BND_WIDGET_HEIGHT * pData->scaleFactor;
    else
        y += 1 * pData->scaleFactor;

    for (std::vector<BlendishMenuItem*>::iterator it = items.begin(); it != items.end(); ++it)
    {
        BlendishMenuItem* const item(*it);

        if (item->isVisible())
        {
            item->setAbsoluteX(x);
            item->setAbsoluteY(y);
            y += item->getHeight();
        }
    }

    nextY = y;

    BlendishSubWidget::onPositionChanged(ev);
}

void BlendishMenu::onResize(const ResizeEvent& ev)
{
    const uint width = ev.size.getWidth();

    for (std::vector<BlendishMenuItem*>::iterator it = items.begin(); it != items.end(); ++it)
    {
        BlendishMenuItem* const item(*it);
        item->setWidth(width);
    }

    BlendishSubWidget::onResize(ev);
}

void BlendishMenu::recheckSize(const uint newItemWidth, const uint newItemHeight)
{
    Size<uint> size(getSize());

    // set width
    if (newItemWidth > getWidth())
        size.setWidth(newItemWidth);

    if (newItemWidth > biggestItemWidth)
        biggestItemWidth = newItemWidth;

    // set height
    if (newItemHeight == 0)
    {
        uint nextHeight = 0;

        if (getLabel() != nullptr)
            nextHeight += BND_WIDGET_HEIGHT * pData->scaleFactor;

        for (std::vector<BlendishMenuItem*>::iterator it = items.begin(); it != items.end(); ++it)
        {
            BlendishMenuItem* const item(*it);
            nextHeight += item->getHeight();
        }

        nextY = getAbsoluteY() + nextHeight;
        size.setHeight(nextHeight);
    }
    else
    {
        size.setHeight((items.size() != 1 || getLabel() != nullptr ? getHeight() : 0) + newItemHeight);
    }

    // done
    setSize(size);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishComboBox::BlendishComboBox(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      menu(this),
      currentIndex(-1),
      defaultLabel(nullptr),
      callback(nullptr)
{
    pData->labelCanChangeWidth = false;
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);

    menu.hide();
    pData->internalCallback = new BlendishMenu::CallbackComboBox(menu);
}

BlendishComboBox::~BlendishComboBox()
{
    std::free(defaultLabel);
}

int BlendishComboBox::getCurrentIndex() const noexcept
{
    return currentIndex;
}

const char* BlendishComboBox::getCurrentLabel() const noexcept
{
    return getLabel();
}

BlendishMenuItem* BlendishComboBox::getCurrentMenuItem() const noexcept
{
    if (currentIndex == -1)
        return nullptr;

    return menu.items[currentIndex];
}

void BlendishComboBox::setCurrentIndex(int index, const bool triggerCallback)
{
    if (index < -1)
        index = -1;
    else if (index > menu.items.size())
        index = menu.items.size();

    if (currentIndex == index)
        return;

    currentIndex = index;
    BlendishSubWidget::setLabel(index == -1 ? defaultLabel : menu.items[index]->getLabel());

    if (triggerCallback && callback != nullptr)
        callback->blendishComboBoxIndexChanged(this, index);
}

void BlendishComboBox::setCurrentLabel(const char* const label)
{
    if (currentIndex == -1)
        setDefaultLabel(label);
    else
        menu.items[currentIndex]->setLabel(label);

    BlendishSubWidget::setLabel(label);
}

void BlendishComboBox::setCurrentMenuItem(BlendishMenuItem* const menuItem, const bool triggerCallback)
{
    DISTRHO_SAFE_ASSERT_RETURN(menuItem != nullptr,);

    int index = 0;
    for (std::vector<BlendishMenuItem*>::iterator it = menu.items.begin(); it != menu.items.end(); ++it, ++index)
    {
        BlendishMenuItem* const item(*it);

        if (item == menuItem)
        {
            if (item->isVisible())
                setCurrentIndex(index, triggerCallback);

            break;
        }
    }
}

const char* BlendishComboBox::getDefaultLabel() const noexcept
{
    return defaultLabel;
}

void BlendishComboBox::setDefaultLabel(const char* const label)
{
    std::free(defaultLabel);

    if (label != nullptr && label[0] != '\0')
    {
        defaultLabel = strdup(label);
        setWidth(std::max((double)menu.biggestItemWidth,
                          bndLabelWidth(pData->context, BND_ICON_NONE, label) * pData->scaleFactor));
    }
    else
    {
        defaultLabel = nullptr;
        setWidth(std::max(menu.biggestItemWidth, static_cast<uint>(BND_TOOL_WIDTH * pData->scaleFactor + 0.5)));
    }

    if (currentIndex == -1)
        BlendishSubWidget::setLabel(label);
}

BlendishMenuItem* BlendishComboBox::addMenuItem(const char* const label)
{
     BlendishMenuItem* const item = menu.addMenuItem(label);
     item->pData->internalCallback = new BlendishMenuItem::CallbackComboBox(this, menu);
     setWidth(std::max(getWidth(), item->getWidth()));
     return item;
}

void BlendishComboBox::setCallback(Callback* const cb)
{
    callback = cb;
}

uint BlendishComboBox::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishComboBox::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndChoiceButton(pData->context, x, y, w, h,
                    static_cast<BNDcornerFlags>(pData->flags),
                    pData->getBlendishState(), -1, pData->label);
}

bool BlendishComboBox::onMouse(const MouseEvent& ev)
{
    if (! menu.contains(ev.pos))
    {
        menu.hide();
        repaint();
    }

    return BlendishSubWidget::onMouse(ev);
}

void BlendishComboBox::setLabel(const char*)
{
}

// --------------------------------------------------------------------------------------------------------------------

BlendishColorButton::BlendishColorButton(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishColorButton::BlendishColorButton(SubWidget* const parent)
    : BlendishSubWidget(parent)
{
    setSize(BND_TOOL_WIDTH*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

uint BlendishColorButton::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

Color BlendishColorButton::getColor() const noexcept
{
    return color;
}

void BlendishColorButton::setColor(const Color c)
{
    color = c;
}

void BlendishColorButton::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndColorButton(pData->context, x, y, w, h, static_cast<BNDcornerFlags>(pData->flags), color);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishNumberField::BlendishNumberField(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      value(0)
{
    setSize(48*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

BlendishNumberField::BlendishNumberField(SubWidget* const parent)
    : BlendishSubWidget(parent),
      value(0)
{
    setSize(48*pData->scaleFactor, BND_WIDGET_HEIGHT*pData->scaleFactor);
}

int BlendishNumberField::getValue() const noexcept
{
    return value;
}

void BlendishNumberField::setValue(const int v)
{
    value = v;
}

uint BlendishNumberField::getMinimumWidth() const noexcept
{
    return 48;
}

void BlendishNumberField::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    char valuestr[32];
    snprintf(valuestr, sizeof(valuestr)-1, "%d", value);

    bndNumberField(pData->context, x, y, w, h,
                   static_cast<BNDcornerFlags>(pData->flags),
                   pData->getBlendishState(), pData->label, valuestr);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishKnob::BlendishKnob(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      min(0.0f),
      max(1.0f),
      def(0.5f),
      value(def)
{
    setSize(64 * pData->scaleFactor, (64 + BND_WIDGET_HEIGHT) * pData->scaleFactor);
}

BlendishKnob::BlendishKnob(SubWidget* const parent)
    : BlendishSubWidget(parent),
      min(0.0f),
      max(1.0f),
      def(0.5f),
      value(def)
{
    setSize(64 * pData->scaleFactor, (64 + BND_WIDGET_HEIGHT) * pData->scaleFactor);
}

void BlendishKnob::setup(const float min2, const float max2, const float def2)
{
    min = min2;
    max = max2;
    def = def2;
}

float BlendishKnob::getCurrentValue() const noexcept
{
    return value;
}

void BlendishKnob::setCurrentValue(const float v)
{
    value = v;
}

uint BlendishKnob::getMinimumWidth() const noexcept
{
    return 64;
}

void BlendishKnob::onBlendishDisplay()
{
    const double scaleFactor = pData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    const BNDcornerFlags flags = static_cast<BNDcornerFlags>(pData->flags);
    const BNDwidgetState state = pData->getBlendishState();
    auto ctx = pData->context;

    char valuestr[32];
    snprintf(valuestr, sizeof(valuestr)-1, "%f", value);

//     float cr[4];
    NVGcolor shade_top, shade_down;

//     bndSelectCorners(cr, BND_NUMBER_RADIUS, flags);
//     bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.numberFieldTheme, state, 0);

    nvgBeginPath(ctx);
    nvgRect(ctx, x, y, w, h);
    nvgFillColor(ctx, Color(0.3f, 0.8f, 0.23f));
    nvgFill(ctx);

    const auto knobSize = std::min(w, h - BND_WIDGET_HEIGHT);

    nvgBeginPath(ctx);
    nvgCircle(ctx, x + knobSize / 2, y + knobSize / 2, knobSize / 2);
    nvgStrokeColor(ctx, shade_down);
    nvgStrokeWidth(ctx, 1.0f);
    nvgStroke(ctx);
    nvgFillColor(ctx, state != BND_DEFAULT ? shade_down : shade_top);
    nvgFill(ctx);

    bndIconLabelValue(ctx, x, y + knobSize, knobSize, BND_WIDGET_HEIGHT, -1,
        bnd_theme.regularTheme.textColor, BND_CENTER,
        BND_LABEL_FONT_SIZE, pData->label, NULL);

#if 0
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.numberFieldTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,-1,
        bndTextColor(&bnd_theme.numberFieldTheme, state), BND_CENTER,
        BND_LABEL_FONT_SIZE, pData->label, valuestr);
    bndArrow(ctx,x+8,y+10,-BND_NUMBER_ARROW_SIZE,
        bndTransparent(bnd_theme.numberFieldTheme.itemColor));
    bndArrow(ctx,x+w-8,y+10,BND_NUMBER_ARROW_SIZE,
        bndTransparent(bnd_theme.numberFieldTheme.itemColor));
#endif
}

// --------------------------------------------------------------------------------------------------------------------
// simple function to group things together, setting corner flags automatically, must be null terminated

uint groupBlendishWidgets(const bool horizontal, BlendishSubWidget* widget...)
{
    BlendishSubWidget* prev = nullptr;
    uint numWidgetsSoFar = 0;
    uint pos = horizontal ? widget->getAbsoluteX() : widget->getAbsoluteY();

    // for vertical stacking
    uint widthSoFar = widget->getWidth();
    std::vector<BlendishSubWidget*> widgets;

    va_list args;
    va_start(args, widget);

    do
    {
        if (horizontal)
        {
            if (prev != nullptr)
            {
                prev->setCornerFlags(prev->getCornerFlags() | BlendishSubWidget::kCornerRight);
                widget->setAbsoluteX(pos - 2*numWidgetsSoFar);
                widget->setCornerFlags(BlendishSubWidget::kCornerLeft);
            }

            pos += widget->getWidth();
        }
        else
        {
            const uint width = widget->getWidth();

            if (width > widthSoFar)
            {
                widthSoFar = width;
                for (BlendishSubWidget* w : widgets)
                    w->setWidth(width);
            }

            if (prev != nullptr)
            {
                prev->setCornerFlags(prev->getCornerFlags() | BlendishSubWidget::kCornerDown);
                widget->setAbsoluteY(pos - 3*numWidgetsSoFar);
                widget->setCornerFlags(BlendishSubWidget::kCornerTop);
            }

            pos += widget->getHeight();
            widgets.push_back(widget);
        }

        prev = widget;
        ++numWidgetsSoFar;
    }
    while ((widget = va_arg(args, BlendishSubWidget*)) != nullptr);

    va_end(args);

    return pos;
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
