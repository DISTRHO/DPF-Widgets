/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2022 Filipe Coelho <falktx@falktx.com>
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

// ImGui is quite large, build it separately
#define IMGUI_DPF_BACKEND
#include "../opengl/DearImGui/imgui.cpp"
#include "../opengl/DearImGui/imgui_demo.cpp"
#include "../opengl/DearImGui/imgui_draw.cpp"
#include "../opengl/DearImGui/imgui_tables.cpp"
#include "../opengl/DearImGui/imgui_widgets.cpp"
#include "../opengl/DearImGuiColorTextEditor/TextEditor.cpp"

#if defined(DGL_USE_GLES2)
# define IMGUI_IMPL_OPENGL_ES2
#elif defined(DGL_USE_GLES3)
# define IMGUI_IMPL_OPENGL_ES3
#endif

# if defined(DGL_USE_GLES2) || defined(DGL_USE_GLES3) || defined(DGL_USE_OPENGL3)
# include "../opengl/DearImGui/imgui_impl_opengl3.cpp"
#else
# include "../opengl/DearImGui/imgui_impl_opengl2.cpp"
#endif
