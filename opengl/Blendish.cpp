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

static BNDwidgetState getBlendishState(const ButtonEventHandler::State state) noexcept
{
    switch (state)
    {
    case ButtonEventHandler::kButtonStateDefault:
        return BND_DEFAULT;
    case ButtonEventHandler::kButtonStateHover:
        return BND_HOVER;
    case ButtonEventHandler::kButtonStateActive:
    case ButtonEventHandler::kButtonStateActiveHover:
        return BND_ACTIVE;
    }

    return BND_DEFAULT;
}

static BNDwidgetState getBlendishState(const KnobEventHandler::State state) noexcept
{
    switch (state)
    {
    case KnobEventHandler::kKnobStateDefault:
        return BND_DEFAULT;
    case KnobEventHandler::kKnobStateHover:
        return BND_HOVER;
    case KnobEventHandler::kKnobStateDragging:
    case KnobEventHandler::kKnobStateDraggingHover:
        return BND_ACTIVE;
    }

    return BND_DEFAULT;
}

// --------------------------------------------------------------------------------------------------------------------

static void blendishSetWidgetTheme(BNDwidgetTheme& wtheme, const BlendishWidgetTheme& wtheme2)
{
    wtheme.outlineColor = wtheme2.outlineColor;
    wtheme.itemColor = wtheme2.itemColor;
    wtheme.innerColor = wtheme2.innerColor;
    wtheme.innerSelectedColor = wtheme2.innerSelectedColor;
    wtheme.textColor = wtheme2.textColor;
    wtheme.textSelectedColor = wtheme2.textSelectedColor;
    wtheme.shadeTop = wtheme2.shadeTop;
    wtheme.shadeDown = wtheme2.shadeDown;
}

void blendishSetTheme(const BlendishTheme& theme2)
{
    BNDtheme theme;
    memset(&theme, 0, sizeof(theme));
    theme.backgroundColor = theme2.backgroundColor;
    blendishSetWidgetTheme(theme.regularTheme, theme2.labelTheme);
    blendishSetWidgetTheme(theme.toolTheme, theme2.toolButtonTheme);
    // blendishSetWidgetTheme(theme.radioButtonTheme, theme2.labelTheme);
    // blendishSetWidgetTheme(theme.textFieldTheme, theme2.radioTheme);
    blendishSetWidgetTheme(theme.optionTheme, theme2.checkBoxTheme);
    blendishSetWidgetTheme(theme.choiceTheme, theme2.comboBoxTheme);
    blendishSetWidgetTheme(theme.numberFieldTheme, theme2.numberFieldTheme);
    // blendishSetWidgetTheme(theme.sliderTheme, theme2.sliderTheme);
    // blendishSetWidgetTheme(theme.scrollBarTheme, theme2.scrollBarTheme);
    // blendishSetWidgetTheme(theme.tooltipTheme, theme2.tooltipTheme);
    blendishSetWidgetTheme(theme.menuTheme, theme2.menuTheme);
    blendishSetWidgetTheme(theme.menuItemTheme, theme2.menuItemTheme);
    // blendishSetWidgetTheme(theme.nodeTheme, theme2.nodeTheme);
    bndSetTheme(theme);
}

const BlendishTheme& blendishGetDefaultTheme()
{
    static const BlendishTheme theme = {
        // backgroundColor
        {{ 0.447, 0.447, 0.447, 1.0 }},
        // labelTheme
        {
            {{ 0.098,0.098,0.098,1 }}, // color_outline
            {{ 0.098,0.098,0.098,1 }}, // color_item
            {{ 0.6,0.6,0.6,1 }}, // color_inner
            {{ 0.392,0.392,0.392,1 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            0, // shade_top
            0, // shade_down
        },
        // toolButtonTheme
        {
            {{ 0.098,0.098,0.098,1 }}, // color_outline
            {{ 0.098,0.098,0.098,1 }}, // color_item
            {{ 0.6,0.6,0.6,1 }}, // color_inner
            {{ 0.392,0.392,0.392,1 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            15, // shade_top
            -15, // shade_down
        },
#if 0
        // radioButtonTheme
        {
            {{ 0,0,0,1 }}, // color_outline
            {{ 1,1,1,1 }}, // color_item
            {{ 0.275,0.275,0.275,1 }}, // color_inner
            {{ 0.337,0.502,0.761,1 }}, // color_inner_selected
            BND_COLOR_TEXT_SELECTED, // color_text
            BND_COLOR_TEXT, // color_text_selected
            15, // shade_top
            -15, // shade_down
        },
        // textFieldTheme
        {
            {{ 0.098,0.098,0.098,1 }}, // color_outline
            {{ 0.353, 0.353, 0.353,1 }}, // color_item
            {{ 0.6, 0.6, 0.6,1 }}, // color_inner
            {{ 0.6, 0.6, 0.6,1 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            0, // shade_top
            25, // shade_down
        },
#endif
        // optionTheme
        {
            {{ 0,0,0,1 }}, // color_outline
            {{ 1,1,1,1 }}, // color_item
            {{ 0.275,0.275,0.275,1 }}, // color_inner
            {{ 0.275,0.275,0.275,1 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            15, // shade_top
            -15, // shade_down
        },
        // choiceTheme
        {
            {{ 0,0,0,1 }}, // color_outline
            {{ 1,1,1,1 }}, // color_item
            {{ 0.275,0.275,0.275,1 }}, // color_inner
            {{ 0.275,0.275,0.275,1 }}, // color_inner_selected
            BND_COLOR_TEXT_SELECTED, // color_text
            {{ 0.8,0.8,0.8,1 }}, // color_text_selected
            15, // shade_top
            -15, // shade_down
        },
        // numberFieldTheme
        {
            {{ 0.098,0.098,0.098,1 }}, // color_outline
            {{ 0.353, 0.353, 0.353,1 }}, // color_item
            {{ 0.706, 0.706, 0.706,1 }}, // color_inner
            {{ 0.6, 0.6, 0.6,1 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            -20, // shade_top
            0, // shade_down
        },
#if 0
        // sliderTheme
        {
            {{ 0.098,0.098,0.098,1 }}, // color_outline
            {{ 0.502,0.502,0.502,1 }}, // color_item
            {{ 0.706, 0.706, 0.706,1 }}, // color_inner
            {{ 0.6, 0.6, 0.6,1 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            -20, // shade_top
            0, // shade_down
        },
        // scrollBarTheme
        {
            {{ 0.196,0.196,0.196,1 }}, // color_outline
            {{ 0.502,0.502,0.502,1 }}, // color_item
            {{ 0.314, 0.314, 0.314,0.706 }}, // color_inner
            {{ 0.392, 0.392, 0.392,0.706 }}, // color_inner_selected
            BND_COLOR_TEXT, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            5, // shade_top
            -5, // shade_down
        },
        // tooltipTheme
        {
            {{ 0,0,0,1 }}, // color_outline
            {{ 0.392,0.392,0.392,1 }}, // color_item
            {{ 0.098, 0.098, 0.098, 0.902 }}, // color_inner
            {{ 0.176, 0.176, 0.176, 0.902 }}, // color_inner_selected
            {{ 0.627, 0.627, 0.627, 1 }}, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            0, // shade_top
            0, // shade_down
        },
#endif
        // menuTheme
        {
            {{ 0,0,0,1 }}, // color_outline
            {{ 0.392,0.392,0.392,1 }}, // color_item
            {{ 0.098, 0.098, 0.098, 0.902 }}, // color_inner
            {{ 0.176, 0.176, 0.176, 0.902 }}, // color_inner_selected
            {{ 0.627, 0.627, 0.627, 1 }}, // color_text
            BND_COLOR_TEXT_SELECTED, // color_text_selected
            0, // shade_top
            0, // shade_down
        },
        // menuItemTheme
        {
            {{ 0,0,0,1 }}, // color_outline
            {{ 0.675,0.675,0.675,0.502 }}, // color_item
            {{ 0,0,0,0 }}, // color_inner
            {{ 0.337,0.502,0.761,1 }}, // color_inner_selected
            BND_COLOR_TEXT_SELECTED, // color_text
            BND_COLOR_TEXT, // color_text_selected
            38, // shade_top
            0, // shade_down
        },
#if 0
        // nodeTheme
        {
            {{ 0.945,0.345,0,1 }}, // nodeSelectedColor
            {{ 0,0,0,1 }}, // wiresColor
            {{ 0.498,0.439,0.439,1 }}, // textSelectedColor
            {{ 1,0.667,0.251,1 }}, // activeNodeColor
            {{ 1,1,1,1 }}, // wireSelectColor
            {{ 0.608,0.608,0.608,0.627 }}, // nodeBackdropColor
            5, // noodleCurving
        },
#endif
    };

    return theme;
}

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

struct BlendishSubWidget::ProtectedData {
    BlendishSubWidget* const self;
    BlendishSubWidgetSharedContext* const shared;
    NanoVG* const nvg;
    NVGcontext* const context;
    double scaleFactor;

    int flags;
    bool labelCanChangeWidth;
    char* label;

    explicit ProtectedData(BlendishSubWidget* const s, BlendishSubWidgetSharedContext* const p)
        : self(s),
          shared(p),
          nvg(nullptr),
          context(p->pData->nvg.getContext()),
          scaleFactor(p->pData->scaleFactor),
          flags(kCornerNone),
          labelCanChangeWidth(true),
          label(nullptr)
    {
        shared->pData->widgets.push_back(self);
    }

    explicit ProtectedData(BlendishSubWidget* const s, BlendishSubWidget* const parent)
        : self(s),
          shared(parent != nullptr ? parent->bData->shared : nullptr),
          nvg(parent != nullptr ? nullptr : new NanoVG()),
          context(parent != nullptr ? parent->bData->context : nvg->getContext()),
          scaleFactor(parent != nullptr ? parent->bData->scaleFactor : s->getWindow().getScaleFactor()),
          flags(kCornerNone),
          labelCanChangeWidth(true),
          label(nullptr) {}

    ~ProtectedData()
    {
        if (shared != nullptr)
            shared->pData->widgets.remove(self);

        delete nvg;
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishSubWidget::BlendishSubWidget(BlendishSubWidgetSharedContext* const parent)
    : SubWidget(parent),
      bData(new ProtectedData(this, parent))
{
    setSkipDrawing(true);
}

BlendishSubWidget::BlendishSubWidget(SubWidget* const parent)
    : SubWidget(parent),
      bData(new ProtectedData(this, dynamic_cast<BlendishSubWidget*>(parent)))
{
    if (bData->shared == nullptr)
        setNeedsViewportScaling(true, bData->scaleFactor);
}

BlendishSubWidget::~BlendishSubWidget()
{
    delete bData;
}

int BlendishSubWidget::getCornerFlags() const noexcept
{
    return bData->flags;
}

void BlendishSubWidget::setCornerFlags(const int flags)
{
    bData->flags = flags;
}

const char* BlendishSubWidget::getLabel() const noexcept
{
    return bData->label;
}

void BlendishSubWidget::setLabel(const char* const label)
{
    std::free(bData->label);

    if (label != nullptr && label[0] != '\0')
    {
        bData->label = strdup(label);

        if (bData->labelCanChangeWidth)
            setWidth((getMinimumWidth() + bndLabelWidth(bData->context, 0, label)) * bData->scaleFactor);
    }
    else
    {
        bData->label = nullptr;

        if (bData->labelCanChangeWidth)
            setWidth(1*bData->scaleFactor);
    }
}

void BlendishSubWidget::toFront()
{
    if (bData->shared != nullptr)
    {
        std::list<BlendishSubWidget*>& widgets(bData->shared->pData->widgets);

        widgets.remove(this);
        widgets.push_back(this);
    }

    SubWidget::toFront();
}

BlendishSubWidgetSharedContext* BlendishSubWidget::getSharedContext()
{
    return bData->shared;
}

void BlendishSubWidget::onDisplay()
{
    if (NanoVG* const nvg = bData->nvg)
    {
        nvg->beginFrame(getWidth(), getHeight(), bData->scaleFactor);
        onBlendishDisplay();
        nvg->endFrame();
    }
}

// --------------------------------------------------------------------------------------------------------------------

BlendishLabel::BlendishLabel(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent)
{
    setSize(1*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishLabel::BlendishLabel(SubWidget* const parent)
    : BlendishSubWidget(parent)
{
    setSize(1*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

uint BlendishLabel::getMinimumWidth() const noexcept
{
    return 1;
}

void BlendishLabel::onBlendishDisplay()
{
    if (bData->label == nullptr)
        return;

    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndLabel(bData->context, x, y, w, h, -1, bData->label);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishToolButton::BlendishToolButton(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishToolButton::BlendishToolButton(SubWidget* const parent)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

uint BlendishToolButton::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishToolButton::onBlendishDisplay()
{
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndToolButton(bData->context, x, y, w, h,
                  static_cast<BNDcornerFlags>(bData->flags),
                  getBlendishState(getState()), -1, bData->label);
}

bool BlendishToolButton::onMouse(const MouseEvent& ev)
{
    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishToolButton::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishCheckBox::BlendishCheckBox(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    ButtonEventHandler::setCheckable(true);
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishCheckBox::BlendishCheckBox(SubWidget* const parent)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    ButtonEventHandler::setCheckable(true);
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

uint BlendishCheckBox::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishCheckBox::onBlendishDisplay()
{
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    BNDwidgetState state;

    if (isChecked())
        state = BND_ACTIVE;
    else if (getState() & kButtonStateHover)
        state = BND_HOVER;
    else
        state = BND_DEFAULT;

    bndOptionButton(bData->context, x, y, w, h, state, bData->label);
}

bool BlendishCheckBox::onMouse(const MouseEvent& ev)
{
    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishCheckBox::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

struct BlendishMenuItem::CallbackComboBox : ButtonEventHandler::Callback
{
    BlendishComboBox* const comboBox;
    BlendishMenu& menu;

    explicit CallbackComboBox(BlendishComboBox* const cb, BlendishMenu& m)
        : comboBox(cb),
          menu(m) {}

    void buttonClicked(SubWidget* const widget, const int button) override
    {
        if (button != 1)
            return;

        menu.hide();

        BlendishMenuItem* const menuItem = dynamic_cast<BlendishMenuItem*>(widget);
        DISTRHO_SAFE_ASSERT_RETURN(menuItem != nullptr,);

        comboBox->setCurrentMenuItem(menuItem, true);
        menuItem->triggerUserCallback(widget, button);
    }
};

// --------------------------------------------------------------------------------------------------------------------

BlendishMenuItem::BlendishMenuItem(BlendishMenu* const parent, const char* const label)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    setMargin(0, static_cast<int>(0.5 * bData->scaleFactor + 0.5));
    setSize(BND_TOOL_WIDTH * bData->scaleFactor, (BND_WIDGET_HEIGHT - 1) * bData->scaleFactor);

    if (label != nullptr)
        setLabel(label);
}

uint BlendishMenuItem::getMinimumWidth() const noexcept
{
    return BND_TOOL_WIDTH;
}

void BlendishMenuItem::onBlendishDisplay()
{
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndMenuItem(bData->context, x, y, w, h, bData->flags,
                getBlendishState(getState()), -1, bData->label);
}

bool BlendishMenuItem::onMouse(const MouseEvent& ev)
{
    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishMenuItem::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

struct BlendishMenu::CallbackComboBox : ButtonEventHandler::Callback
{
    BlendishMenu& menu;

    explicit CallbackComboBox(BlendishMenu& m)
        : menu(m) {}

    void buttonClicked(SubWidget* const widget, const int button) override
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
      labelAtBottom(false),
      oldMotionPos()
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishMenu::BlendishMenu(BlendishComboBox* const parent)
    : BlendishSubWidget(parent->getSharedContext()),
      items(),
      lastHoveredItem(nullptr),
      matchingComboBox(parent),
      biggestItemWidth(0),
      nextY(0),
      labelAtBottom(false),
      oldMotionPos()
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishMenu::BlendishMenu(SubWidget* const parent)
    : BlendishSubWidget(parent),
      items(),
      lastHoveredItem(nullptr),
      matchingComboBox(nullptr),
      biggestItemWidth(0),
      nextY(0),
      labelAtBottom(false),
      oldMotionPos()
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
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
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndMenuBackground(bData->context, x, y, w, h, bData->flags);

    if (bData->label != nullptr)
        bndMenuLabel(bData->context,
                     x, labelAtBottom ? (h - BND_WIDGET_HEIGHT - 2) : y,
                     w, BND_WIDGET_HEIGHT, -1, bData->label);

    for (std::vector<BlendishMenuItem*>::iterator it = items.begin(); it != items.end(); ++it)
    {
        BlendishMenuItem* const item(*it);

        if (item->isVisible())
            item->onBlendishDisplay();
    }
}

bool BlendishMenu::onMotion(const MotionEvent& ev)
{
    const bool containsOldPos = contains(oldMotionPos);
    oldMotionPos = ev.pos;

    if (BlendishSubWidget::onMotion(ev))
        return true;

    if (contains(ev.pos) && ! containsOldPos)
    {
        if (matchingComboBox != nullptr)
            matchingComboBox->clearState();
        return true;
    }

    /*
    void stateChanged(const State state, const State oldState) override
    {
        const bool  isHovering = state    & kButtonStateHover;
        const bool wasHovering = oldState & kButtonStateHover;

        if (isHovering && ! wasHovering)
            if (BlendishMenu* const menu = dynamic_cast<BlendishMenu*>(self))
                if (menu->matchingComboBox != nullptr)
                    menu->matchingComboBox->pData->clearState();
    }
    */

    return false;
}

void BlendishMenu::onPositionChanged(const PositionChangedEvent& ev)
{
    const int x = ev.pos.getX();
    /* */ int y = ev.pos.getY();

    if (getLabel() != nullptr)
        y += BND_WIDGET_HEIGHT * bData->scaleFactor;
    else
        y += 1 * bData->scaleFactor;

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
            nextHeight += BND_WIDGET_HEIGHT * bData->scaleFactor;

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
      ButtonEventHandler(this),
      menu(this),
      currentIndex(-1),
      defaultLabel(nullptr),
      callback(nullptr)
{
    bData->labelCanChangeWidth = false;
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);

    menu.hide();
    internalCallback = new BlendishMenu::CallbackComboBox(menu);
    setInternalCallback(internalCallback);
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
    else if ((uint)index > menu.items.size())
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
                          bndLabelWidth(bData->context, BND_ICON_NONE, label) * bData->scaleFactor));
    }
    else
    {
        defaultLabel = nullptr;
        setWidth(std::max(menu.biggestItemWidth, static_cast<uint>(BND_TOOL_WIDTH * bData->scaleFactor + 0.5)));
    }

    if (currentIndex == -1)
        BlendishSubWidget::setLabel(label);
}

BlendishMenuItem* BlendishComboBox::addMenuItem(const char* const label)
{
     BlendishMenuItem* const item = menu.addMenuItem(label);
     item->internalCallback = new BlendishMenuItem::CallbackComboBox(this, menu);
     item->setInternalCallback(item->internalCallback);
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
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndChoiceButton(bData->context, x, y, w, h,
                    static_cast<BNDcornerFlags>(bData->flags),
                    getBlendishState(getState()), -1, bData->label);
}

bool BlendishComboBox::onMouse(const MouseEvent& ev)
{
    if (menu.isVisible() && ! menu.contains(ev.pos))
    {
        menu.hide();
        repaint();
    }

    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishComboBox::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishColorButton::BlendishColorButton(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishColorButton::BlendishColorButton(SubWidget* const parent)
    : BlendishSubWidget(parent),
      ButtonEventHandler(this)
{
    setSize(BND_TOOL_WIDTH*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
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
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    bndColorButton(bData->context, x, y, w, h, static_cast<BNDcornerFlags>(bData->flags), color);
}

bool BlendishColorButton::onMouse(const MouseEvent& ev)
{
    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishColorButton::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishNumberField::BlendishNumberField(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      KnobEventHandler(this)
{
    setStep(1.0f);
    setRange(0.0f, 100.0f);
    setValue(50.0f, false);
    setSize(48*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

BlendishNumberField::BlendishNumberField(SubWidget* const parent)
    : BlendishSubWidget(parent),
      KnobEventHandler(this)
{
    setStep(1.0f);
    setRange(0.0f, 100.0f);
    setValue(50.0f, false);
    setSize(48*bData->scaleFactor, BND_WIDGET_HEIGHT*bData->scaleFactor);
}

uint BlendishNumberField::getMinimumWidth() const noexcept
{
    return 48;
}

void BlendishNumberField::onBlendishDisplay()
{
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    char valuestr[32];
    snprintf(valuestr, sizeof(valuestr)-1, "%d", static_cast<int>(getValue()));

    bndNumberField(bData->context, x, y, w, h,
                   static_cast<BNDcornerFlags>(bData->flags),
                   getBlendishState(getState()), bData->label, valuestr);
}

bool BlendishNumberField::onMouse(const MouseEvent& ev)
{
    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishNumberField::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

bool BlendishNumberField::onScroll(const ScrollEvent& ev)
{
    if (BlendishSubWidget::onScroll(ev))
        return true;
    return scrollEvent(ev);
}

// --------------------------------------------------------------------------------------------------------------------

BlendishKnob::BlendishKnob(BlendishSubWidgetSharedContext* const parent)
    : BlendishSubWidget(parent),
      KnobEventHandler(this)
{
    setSize(64 * bData->scaleFactor, (64 + BND_WIDGET_HEIGHT) * bData->scaleFactor);
}

BlendishKnob::BlendishKnob(SubWidget* const parent)
    : BlendishSubWidget(parent),
      KnobEventHandler(this)
{
    setSize(64 * bData->scaleFactor, (64 + BND_WIDGET_HEIGHT) * bData->scaleFactor);
}

uint BlendishKnob::getMinimumWidth() const noexcept
{
    return 64;
}

void BlendishKnob::onBlendishDisplay()
{
    const double scaleFactor = bData->scaleFactor;
    const float x = getAbsoluteX() / scaleFactor;
    const float y = getAbsoluteY() / scaleFactor;
    const float w = getWidth() / scaleFactor;
    const float h = getHeight() / scaleFactor;

    // const BNDcornerFlags flags = static_cast<BNDcornerFlags>(bData->flags);
    const BNDwidgetState state = BND_DEFAULT; // TODO bData->getBlendishState();
    auto ctx = bData->context;

    char valuestr[32];
    snprintf(valuestr, sizeof(valuestr)-1, "%f", getValue());

//     float cr[4];
    NVGcolor shade_top, shade_down;

//     bndSelectCorners(cr, BND_NUMBER_RADIUS, flags);
//     bndBevelInset(ctx,x,y,w,h,cr[2],cr[3]);
    bndInnerColors(&shade_top, &shade_down, &bnd_theme.menuTheme, state, 0);

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
        BND_LABEL_FONT_SIZE, bData->label, NULL);

#if 0
    bndInnerBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3], shade_top, shade_down);
    bndOutlineBox(ctx,x,y,w,h,cr[0],cr[1],cr[2],cr[3],
        bndTransparent(bnd_theme.numberFieldTheme.outlineColor));
    bndIconLabelValue(ctx,x,y,w,h,-1,
        bndTextColor(&bnd_theme.numberFieldTheme, state), BND_CENTER,
        BND_LABEL_FONT_SIZE, bData->label, valuestr);
    bndArrow(ctx,x+8,y+10,-BND_NUMBER_ARROW_SIZE,
        bndTransparent(bnd_theme.numberFieldTheme.itemColor));
    bndArrow(ctx,x+w-8,y+10,BND_NUMBER_ARROW_SIZE,
        bndTransparent(bnd_theme.numberFieldTheme.itemColor));
#endif
}

bool BlendishKnob::onMouse(const MouseEvent& ev)
{
    if (BlendishSubWidget::onMouse(ev))
        return true;
    return mouseEvent(ev);
}

bool BlendishKnob::onMotion(const MotionEvent& ev)
{
    if (BlendishSubWidget::onMotion(ev))
        return true;
    return motionEvent(ev);
}

bool BlendishKnob::onScroll(const ScrollEvent& ev)
{
    if (BlendishSubWidget::onScroll(ev))
        return true;
    return scrollEvent(ev);
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
