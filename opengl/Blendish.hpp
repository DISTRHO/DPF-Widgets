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

struct NVGcontext;

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

/** Base class for Blendish widget. */
// TODO allow TopLevelWidget usage, template etc
class BlendishSubWidget : public SubWidget
{
public:
    /** Constructor for a BlendishSubWidget */
    explicit BlendishSubWidget(Widget* parent);

    ~BlendishSubWidget() override;

protected:
    void onDisplay() override;

private:
    struct PrivateData;
    PrivateData* const pData;
    friend class BlendishIndividualWidgetBase;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishSubWidget)
};

// --------------------------------------------------------------------------------------------------------------------

// common blendish widgets code
// special drawing handling, for reusing nanovg context among all widgets
class BlendishIndividualWidgetBase : public SubWidget
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

    explicit BlendishIndividualWidgetBase(BlendishSubWidget* parent);
    ~BlendishIndividualWidgetBase() override;

    // must be reimplemented
    virtual uint getMinimumWidth() const noexcept = 0;

    int getCornerFlags() const noexcept;
    void setCornerFlags(int flags);

    // returns null if label is empty
    const char* getLabel() const noexcept;

    // will change width automatically
    void setLabel(const char* label);

protected:
    // new function for subclasses to override
    virtual void onBlendishDisplay() = 0;

    // common handling
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    // available to subclasses
    NVGcontext* const context;
    double scaleFactor;

    // common handling
    int button;
    int flags;
    int state;
    char* label;

private:
    BlendishSubWidget* const parent;
    friend class BlendishSubWidget;

    // should not be used
    void onDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishIndividualWidgetBase)
};

// --------------------------------------------------------------------------------------------------------------------

class BlendishLabel : public BlendishIndividualWidgetBase
{
public:
    explicit BlendishLabel(BlendishSubWidget* parent);

    uint getMinimumWidth() const noexcept override;

protected:
    void onBlendishDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishLabel)
};

// --------------------------------------------------------------------------------------------------------------------

class BlendishToolButton : public BlendishIndividualWidgetBase
{
public:
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void blendishButtonClicked(BlendishToolButton* blendishButton, int button) = 0;
    };

    explicit BlendishToolButton(BlendishSubWidget* parent);

    uint getMinimumWidth() const noexcept override;

protected:
    void onBlendishDisplay() override;

private:
    Callback* callback;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishToolButton)
};

// --------------------------------------------------------------------------------------------------------------------

/* TODO
 * radio button, as a group?
 * text field
 * option button
 * choice button (checkbox?)
 * color button
 * slider
 * scrollbar
 * menu stuff
 * tooltip stuff
 * node stuff
 * splitter / join area
 */

// --------------------------------------------------------------------------------------------------------------------

class BlendishNumberField : public BlendishIndividualWidgetBase
{
public:
    explicit BlendishNumberField(BlendishSubWidget* parent);

    uint getMinimumWidth() const noexcept override;

    int getValue() const noexcept;
    void setValue(int value);

protected:
    void onBlendishDisplay() override;

private:
    int value;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishNumberField)
};

// --------------------------------------------------------------------------------------------------------------------

// simple function to group things together, setting corner flags automatically, must be null terminated
// if horizontal:
//  - returns total width, does not modify widgets width or height
// if vertical:
//  - returns total height, modifies widgets width to ensure all of the same size, does not widgets height
uint groupBlendishWidgets(bool horizontal, BlendishIndividualWidgetBase* widget...);

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
