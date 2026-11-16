#pragma once

#include "DearImGui/imgui.h"

// Helper: ImOffsetRect A set of offsets to apply to an ImRect.
struct IMGUI_API ImOffsetRect
{
    union
    {
        float Offsets[4];

        struct
        {
            float       Top;
            float       Left;
            float       Bottom;
            float       Right;
        };
    };

    constexpr ImOffsetRect()                                                    : Top(0.0f), Left(0.0f), Bottom(0.0f), Right(0.0f)                  {}
    constexpr ImOffsetRect(const ImVec2& topLeft, const ImVec2& bottomRight)    : ImOffsetRect(topLeft.y, topLeft.x, bottomRight.y, bottomRight.x)  {}
    constexpr ImOffsetRect(const ImVec4& v)                                     : ImOffsetRect(v.x, v.y, v.z, v.w)                                  {}
    constexpr ImOffsetRect(float top, float left, float bottom, float right)    : Top(top), Left(left), Bottom(bottom), Right(right)                {}
    constexpr ImOffsetRect(float all)                                           : Top(all), Left(all), Bottom(all), Right(all)                      {}

    ImVec2      GetSize() const { return ImVec2(Left + Right, Top + Bottom); }
    float       GetWidth() const { return Left + Right; }
    float       GetHeight() const { return Top + Bottom; }
    float       GetAverage() const { return (Top + Left + Bottom + Right) / 4.0f; }
    ImOffsetRect MirrorHorizontally() const { return ImOffsetRect(Top, Right, Bottom, Left); }
    ImOffsetRect MirrorVertically() const { return ImOffsetRect(Bottom, Left, Top, Right); }
    ImOffsetRect Mirror() const { return ImOffsetRect(Bottom, Right, Top, Left); }
};

// Helpers: ImOffsetRect operators
IM_MSVC_RUNTIME_CHECKS_OFF
static inline ImOffsetRect operator+(const ImOffsetRect& lhs, const ImOffsetRect& rhs) { return ImOffsetRect(lhs.Top + rhs.Top, lhs.Left + rhs.Left, lhs.Bottom + rhs.Bottom, lhs.Right + rhs.Right); }
static inline ImOffsetRect operator-(const ImOffsetRect& lhs, const ImOffsetRect& rhs) { return ImOffsetRect(lhs.Top - rhs.Top, lhs.Left - rhs.Left, lhs.Bottom - rhs.Bottom, lhs.Right - rhs.Right); }
static inline ImOffsetRect operator*(const ImOffsetRect& lhs, const ImOffsetRect& rhs) { return ImOffsetRect(lhs.Top * rhs.Top, lhs.Left * rhs.Left, lhs.Bottom * rhs.Bottom, lhs.Right * rhs.Right); }

// Lerp helper: ImOffsetRect counterpart to ImGui's ImLerp().
//
// This overload was introduced alongside the ImGui 1.93.0 upgrade (dpf-widgets
// "DearImGui: Update ImGui backend to 1.93.0"), which changed the generic ImLerp()
// template in imgui_internal.h:
//   // before 1.93.0 - only required T to support +, - and * with a float
//   template<typename T> T ImLerp(T a, T b, float t) { return (T)(a + (b - a) * t); }
//   // 1.93.0 onwards - forces a conversion of a and (b - a) to float
//   template<typename T> T ImLerp(T a, T b, float t) { return (T)((float)a + (float)(b - a) * t); }
//
// The old form worked for ImOffsetRect through the operator- / operator* overloads
// below (the float t implicitly converting via ImOffsetRect(float)); the new form
// requires a (float) conversion operator, which ImOffsetRect deliberately does not
// provide (it is not a scalar). Without this overload, calls such as
// ImLerp(off.KnobInset, on.KnobInset, t) in imgui_toggle_renderer.cpp fail to
// compile with "cannot convert 'ImOffsetRect' to 'float'".
//
// Keep the component-wise semantics identical to the old template's behaviour.
static inline ImOffsetRect ImLerp(const ImOffsetRect& a, const ImOffsetRect& b, float t)
{
    return ImOffsetRect(
        a.Top + (b.Top - a.Top) * t,
        a.Left + (b.Left - a.Left) * t,
        a.Bottom + (b.Bottom - a.Bottom) * t,
        a.Right + (b.Right - a.Right) * t);
}
IM_MSVC_RUNTIME_CHECKS_RESTORE
