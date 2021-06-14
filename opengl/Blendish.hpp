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

#pragma once

#include "SubWidget.hpp"
#include "Color.hpp"

#include <vector>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

/** Base class for Blendish widget. */
// TODO allow TopLevelWidget usage, template etc
class BlendishSubWidgetSharedContext : public SubWidget
{
public:
    /** Constructor for a BlendishSubWidgetSharedContext */
    explicit BlendishSubWidgetSharedContext(Widget* parent);

    ~BlendishSubWidgetSharedContext() override;

    void setScaleFactor(double scaleFactor);

protected:
    void onDisplay() override;

private:
    struct PrivateData;
    PrivateData* const pData;
    friend class BlendishSubWidget;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishSubWidgetSharedContext)
};

// --------------------------------------------------------------------------------------------------------------------

// common blendish widgets code
class BlendishSubWidget : public SubWidget
{
public:
    // flags indicating which corners are sharp (for grouping widgets)
    enum CornerFlags {
        // all corners are round
        kCornerNone = 0x0,
        // sharp top left corner
        kCornerTopLeft = 0x1,
        // sharp top right corner
        kCornerTopRight = 0x2,
        // sharp bottom right corner
        kCornerDownRight = 0x4,
        // sharp bottom left corner
        kCornerDownLeft = 0x8,
        // top border is sharp
        kCornerTop = kCornerTopLeft | kCornerTopRight,
        // bottom border is sharp
        kCornerDown = kCornerDownRight | kCornerDownLeft,
        // left border is sharp
        kCornerLeft = kCornerTopLeft | kCornerDownLeft,
        // right border is sharp
        kCornerRight = kCornerTopRight | kCornerDownRight,
        // all corners are sharp;
        // you can invert a set of flags using ^= kCornerALL
        kCornerALL = kCornerTopLeft | kCornerTopRight | kCornerDownRight | kCornerDownLeft
    };

    struct Callback {
        virtual ~Callback() {}
        virtual void blendishWidgetClicked(BlendishSubWidget* widget, int button) = 0;
    };

    // special drawing handling, for reusing nanovg context among all widgets
    explicit BlendishSubWidget(BlendishSubWidgetSharedContext* parent);

    // regular subwidget, creates new nanovg context
    explicit BlendishSubWidget(SubWidget* parent);

    ~BlendishSubWidget() override;

    int getCornerFlags() const noexcept;
    void setCornerFlags(int flags);

    // returns null if label is empty
    const char* getLabel() const noexcept;

    // will change width automatically
    virtual void setLabel(const char* label);

    void toFront() override;

    void setCallback(Callback* callback);

protected:
    // must be reimplemented
    virtual uint getMinimumWidth() const noexcept = 0;

    // new function for subclasses to override
    virtual void onBlendishDisplay() = 0;

    // common handling
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    // available to subclasses
    struct ProtectedData;
    ProtectedData* const pData;

private:
    // should not be used
    void onDisplay() override;

    friend class BlendishSubWidgetSharedContext;
    friend class BlendishMenuItem;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishSubWidget)
};

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish Label class.

   This widget is a simple text label running on a Blendish context.
   It can also have an icon.

   Uses the label methods from BlendishSubWidget to set its contents.
 */
class BlendishLabel : public BlendishSubWidget
{
public:
    explicit BlendishLabel(BlendishSubWidgetSharedContext* parent);
    explicit BlendishLabel(SubWidget* parent);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishLabel)
};

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish Tool Button class.

   This widget is a simple (tool) button.
   It can have an icon and label, plus rounded corners when part of a group.

   Uses the label methods from BlendishSubWidget to set its contents.
   Will trigger Callback::blendishButtonClicked.
 */
class BlendishToolButton : public BlendishSubWidget
{
public:
    explicit BlendishToolButton(BlendishSubWidgetSharedContext* parent);
    explicit BlendishToolButton(SubWidget* parent);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishToolButton)
};

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish Check-Box class.

   This widget is a simple checkbox.

   Uses the label methods from BlendishSubWidget to set its contents.
   Provides its own methods for setting the checked state.
   Will trigger Callback::blendishButtonClicked.
 */
class BlendishCheckBox : public BlendishSubWidget
{
public:
    explicit BlendishCheckBox(BlendishSubWidgetSharedContext* parent);
    explicit BlendishCheckBox(SubWidget* parent);

    bool isChecked() const noexcept;
    void setChecked(bool checked);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishCheckBox)
};

// --------------------------------------------------------------------------------------------------------------------

class BlendishMenu;

/**
   Blendish menu item class.

   This widget is a menu item, which belongs to a menu.
   It can have an icon and label.

   Uses the label methods from BlendishSubWidget to set its contents.
   Will trigger Callback::blendishButtonClicked.

   @see BlendishMenu
 */
class BlendishMenuItem : public BlendishSubWidget
{
public:
    explicit BlendishMenuItem(BlendishMenu* parent, const char* label = nullptr);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

private:
    struct CallbackComboBox;
    friend class BlendishComboBox;
    friend class BlendishMenu;
    friend class BlendishSubWidget;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishMenuItem)
};

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish menu container class.

   This widget is a simple menu, which uses menu items as children.
   It can have an icon and label (which will be used as menu title) plus rounded corners.

   Uses the label methods from BlendishSubWidget to set menu title, can be empty for no title.
   Will not trigger any callbacks, but the child menu items will.

   @see BlendishMenuItem
 */
class BlendishMenu : public BlendishSubWidget
{
public:
    explicit BlendishMenu(BlendishSubWidgetSharedContext* parent);
    explicit BlendishMenu(SubWidget* parent);

    BlendishMenuItem* addMenuItem(const char* label);

    void setLabel(const char* label) override;

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;
    bool onMotion(const MotionEvent& ev) override;
    void onPositionChanged(const PositionChangedEvent& ev) override;
    void onResize(const ResizeEvent& ev) override;

private:
    struct CallbackComboBox;
    friend class BlendishComboBox;
    friend class BlendishSubWidget;

    std::vector<BlendishMenuItem*> items;
    BlendishMenuItem* lastHoveredItem;
    uint biggestItemWidth;
    int nextY;
    bool labelAtBottom;

    void recheckSize(uint newItemWidth, uint newItemHeight);

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishMenu)
};

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish Combo-Box class.

   This widget is a simple combobox, which will open a menu when clicked.
   It can have an icon and label, plus rounded corners when part of a group.

   Uses the label methods from BlendishSubWidget to set its contents.
   Provides its own methods for adding menu items, and those will trigger Callback::blendishButtonClicked.
 */
class BlendishComboBox : public BlendishSubWidget
{
public:
    struct Callback {
        virtual ~Callback() {}
        virtual void blendishComboBoxIndexChanged(BlendishComboBox* comboBox, int index) = 0;
    };

    explicit BlendishComboBox(BlendishSubWidgetSharedContext* parent);
    explicit BlendishComboBox(SubWidget* parent);
    ~BlendishComboBox() override;

    int getCurrentIndex() const noexcept;
    const char* getCurrentLabel() const noexcept; // same as getLabel
    BlendishMenuItem* getCurrentMenuItem() const noexcept;

    void setCurrentIndex(int index, bool triggerCallback);
    void setCurrentLabel(const char* label);
    void setCurrentMenuItem(BlendishMenuItem* menuItem, bool triggerCallback);

    const char* getDefaultLabel() const noexcept;
    void setDefaultLabel(const char* label);

    BlendishMenuItem* addMenuItem(const char* label);

    void setCallback(Callback* callback);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;
    bool onMouse(const MouseEvent& ev) override;

private:
    BlendishMenu menu;
    int currentIndex;
    char* defaultLabel;
    Callback* callback;

    // should not be used
    void setLabel(const char* label) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishComboBox)
};

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish Color Button class.

   This widget is a color button, which paints itself fully with a given color.
   It can have rounded corners when part of a group.

   Provides its own methods for setting and getting current color.
   Will trigger Callback::blendishButtonClicked.
 */
class BlendishColorButton : public BlendishSubWidget
{
public:
    explicit BlendishColorButton(BlendishSubWidgetSharedContext* parent);
    explicit BlendishColorButton(SubWidget* parent);

    Color getColor() const noexcept;
    void setColor(Color color);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

private:
    Color color;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishColorButton)
};

// --------------------------------------------------------------------------------------------------------------------

/* TODO
 * radio button, as a group?
 * text field
 * slider
 * scrollbar
 * menu stuff
 * tooltip stuff
 * node stuff
 * splitter / join area
 */

// --------------------------------------------------------------------------------------------------------------------

/**
   Blendish Number Field class.

   This widget is a slider-like value,
   with centered text and a small button on each side for decreasing and increasing the current value.
   It can a label, plus rounded corners when part of a group.

   Uses the label methods from BlendishSubWidget to set its contents.
   Provides its own methods for setting and getting current value.
   Will trigger ...
 */
class BlendishNumberField : public BlendishSubWidget
{
public:
    explicit BlendishNumberField(BlendishSubWidgetSharedContext* parent);
    explicit BlendishNumberField(SubWidget* parent);

    int getValue() const noexcept;
    void setValue(int value);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

private:
    int value;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishNumberField)
};

/**
   Blendish Knob class.

   This widget is a custom dial/knob-like control, following blendish style.
   It does not follow blendish motto of copying blender 2.6 widget design,
   instead this is hand-crafted to look similar to other widgets in this collection.

   Uses the label methods from BlendishSubWidget to set its contents.
   Provides its own methods for setting and getting minimum, maximum, default and current value.
   Will trigger ...
 */
class BlendishKnob : public BlendishSubWidget
{
public:
    explicit BlendishKnob(BlendishSubWidgetSharedContext* parent);
    explicit BlendishKnob(SubWidget* parent);

    void setup(float min, float max, float def);

    float getCurrentValue() const noexcept;
    void setCurrentValue(float value);

protected:
    uint getMinimumWidth() const noexcept override;
    void onBlendishDisplay() override;

private:
    float min, max, def, value;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishKnob)
};

// --------------------------------------------------------------------------------------------------------------------

// simple function to group things together, setting corner flags automatically, must be null terminated
// if horizontal:
//  - returns total width, does not modify widgets width or height
// if vertical:
//  - returns total height, modifies widgets width to ensure all of the same size, does not widgets height
uint groupBlendishWidgets(bool horizontal, BlendishSubWidget* widget...);

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
