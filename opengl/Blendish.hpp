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
    explicit BlendishIndividualWidgetBase(BlendishSubWidget* parent);
    ~BlendishIndividualWidgetBase() override;

protected:
    // new function for subclasses to override
    virtual void onBlendishDisplay() = 0;

    // available to subclasses
    NVGcontext* const context;
    double scaleFactor;

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

    // returns null if label is empty
    const char* getLabel() const;

    // will change width automatically
    void setLabel(const char* label);

protected:
    void onBlendishDisplay() override;

private:
    char* label;

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

    // returns null if label is empty
    const char* getLabel() const;

    // will change width automatically
    void setLabel(const char* label);

protected:
    void onBlendishDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

private:
    Callback* callback;
    int button;
    int state;
    char* label;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlendishToolButton)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
