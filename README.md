# DPF Widgets
## Official and Community made reusable GUI widgets for [DPF](https://github.com/DISTRHO/DPF/)

This is a repository for shared GUI widgets targetting [DPF](https://github.com/DISTRHO/DPF/).

Since [DPF](https://github.com/DISTRHO/DPF/) allows multiple backends (cairo, opengl or vulkan), we need to split them as such.
Each folder provides widgets for its dedicated backend type.
It is not mandatory that a widget is usable for more than 1 backend.
Generic widgets (those based on DPF core classes like Color, Rectangle, etc) are placed under the "generic" directory.

## Status

#### generic / ResizeHandle

Resize handle for DPF windows, will sit on bottom-right.

Works in both Cairo and OpenGL modes (classic/legacy OpenGL only, does not support OpenGL3 mode).

Used very often and in many plugins.

---

#### generic / LVGL

![screenshot](https://raw.githubusercontent.com/DISTRHO/lvgl-template-plugin/main/Screenshot.png)

Exposes the [LVGL](https://github.com/lvgl/lvgl) drawing API inside a DGL Widget.
This class will take care of setting up LVGL for drawing, and also user input, resizes and everything in between.

See [lvgl-template-plugin](https://github.com/DISTRHO/lvgl-template-plugin/) for a CMake-based template plugin project around LVGL.

---

#### opengl / Blendish

![screenshot](screenshots/Blendish.png)

[oui-blendish](https://github.com/VCVRack/oui-blendish) widgets for DPF.
Work in progress, usable in very select cases.

Used in:

- [AIDA-X](https://github.com/AidaDSP/AIDA-X/) (standalone options)

---

#### opengl / DearImGui

![screenshot](https://raw.githubusercontent.com/DISTRHO/dear-plugins/main/plugins/ImGuiDemo/Screenshot.png)

Exposes the [Dear ImGui](https://github.com/ocornut/imgui/) drawing API inside a DGL Widget.
The drawing function `onDisplay()` is implemented internally but a new `onImGuiDisplay()` needs to be overridden instead.
This class will take care of setting up ImGui for drawing, and also also user input, resizes and everything in between.

Used in:

- [dear-plugins](https://github.com/DISTRHO/dear-plugins)
- [Ildaeil](https://github.com/DISTRHO/Ildaeil)
- [master_me](https://github.com/trummerschlunk/master_me/) (histogram display)
- [WSTD CRSHR](https://github.com/Wasted-Audio/wstd-crshr)
- [WSTD DLAY](https://github.com/Wasted-Audio/wstd-dlay)
- [WSTD DL3Y](https://github.com/Wasted-Audio/wstd-dl3y)
- [WSTD EQ](https://github.com/Wasted-Audio/wstd-eq)
- [WSTD 3Q](https://github.com/Wasted-Audio/wstd-3q)
- [WSTD FLANGR](https://github.com/Wasted-Audio/wstd-flangr)
- [WSTD FL3NGR](https://github.com/Wasted-Audio/wstd-fl3ngr)
- [WSTD FLDR](https://github.com/Wasted-Audio/wstd-fldr)
- [WSTD MANGLR](https://github.com/Wasted-Audio/wstd-manglr)
- [WSTD M3NGLR](https://github.com/Wasted-Audio/wstd-m3nglr)
- [WSTD SMTHR](https://github.com/Wasted-Audio/wstd-smthr)

See [imgui-template-plugin](https://github.com/DISTRHO/imgui-template-plugin/) for a CMake-based template plugin project around ImGui.
See [imgui-template-app](https://github.com/DISTRHO/imgui-template-app/) for a standalone application template.

---

#### opengl / DearImGuiColorTextEditor

![screenshot](https://raw.githubusercontent.com/DISTRHO/dear-plugins/main/plugins/TextEditor/Screenshot.png)

Text Editor Widget class, based on [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit/).

Used in:

- [dear-plugins](https://github.com/DISTRHO/dear-plugins)

---

#### opengl / Quantum

![screenshot](https://raw.githubusercontent.com/trummerschlunk/master_me/master/img/screenshot-expert.png)

[Quanta](https://forum.cockos.com/showthread.php?t=269437)-inspired widgets for DPF.

Used in:

- [master_me](https://github.com/trummerschlunk/master_me/)

---

## Rules and Layout

Each widget MUST follow these rules:

 1. contain exactly 1 header file, with hpp extension
 2. must subclass SubWidget (for cairo use CairoSubWidget, nanovg should use NanoSubWidget) or TopLevelWidget
 3. widget name must match filename
 4. must have DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName) at the end of its class definition
 5. filenames must avoid generic terms such as widget
 6. contain 0 or 1 implementation files, with cpp extension and same basename as the matching hpp file
 7. optionally contain extra files, directly included from the cpp file
 8. place any extra files under a directory with the same basename as the cpp file, minus the file extension
 9. try to be self-contained, not needing externally installed dependencies (besides the locally available extra files defined in the previous point)
10. be contained within DGL_NAMESPACE
11. have clear documentation specifying its use
12. be cross-platform (win32 and posix)
13. be liberally licensed (the same way as DPF or with a compatible license allowing commercial use)

Coding style rules yet to be defined.
