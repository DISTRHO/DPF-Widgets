// DarkBreeze widgets for DPF
// Copyright (C) 2025 Filipe Coelho <falktx@falktx.com>
// SPDX-License-Identifier: ISC

// NOTE this file is WORK-IN-PROGRESS

#pragma once

#include "EventHandlers.hpp"
#include "NanoVG.hpp"

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

class DarkBreeze : public NanoTopLevelWidget
{
    const Color colorActive, colorInactive;
    bool active = false;

public:
    explicit DarkBreeze(Window& window);

    inline bool isActive() const noexcept
    {
        return active;
    }

    void setActive(bool active);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarkBreeze)
};

// --------------------------------------------------------------------------------------------------------------------

class DarkBreezeButton : public NanoSubWidget,
                         public ButtonEventHandler
{
    char* label = nullptr;

public:
    explicit DarkBreezeButton(DarkBreeze* parent);
    ~DarkBreezeButton() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void setLabel(const char* label);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarkBreezeButton)
};

// --------------------------------------------------------------------------------------------------------------------

class DarkBreezeCheckBox : public NanoSubWidget,
                           public ButtonEventHandler
{
    char* label = nullptr;

public:
    explicit DarkBreezeCheckBox(DarkBreeze* parent);
    ~DarkBreezeCheckBox() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void setLabel(const char* label);

protected:
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarkBreezeCheckBox)
};

// --------------------------------------------------------------------------------------------------------------------

class DarkBreezeLabel : public NanoSubWidget
{
    char* label = nullptr;

public:
    explicit DarkBreezeLabel(DarkBreeze* parent);
    ~DarkBreezeLabel() override;

    inline const char* getLabel() const noexcept
    {
        return label;
    }

    void setLabel(const char* label);

protected:
    void onNanoDisplay() override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DarkBreezeLabel)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
