# DPF Widgets 
## Official and Community made reusable GUI widgets for DPF

This is a repository for shared GUI widgets targetting DPF.
Very much work in progress! Do not use right now unless you plan to contribute some yourself.

Since DPF allows multiple backends (cairo, opengl or vulkan), we need to split them as such.
Each folder provides widgets for its dedicated backend type.
It is not mandatory that a widget is usable for more than 1 backend.
Generic widgets (those based on DPF core classes like Color, Rectangle, etc) are placed under the "generic" directory.

### Rules and Layout

Each widget MUST follow these rules:

 1. contain exactly 1 header file, with hpp extension
 2. must subclass SubWidget (for cairo use CairoSubWidget, nanovg should use NanoSubWidget) or TopLevelWidget
 3. widget name must match filename
 4. must have DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassName) at the end of its class definition
 5. filenames must avoid generic terms such as widget
 6. contain 0 or 1 implementation files, with cpp extension and same basename as the matching hpp file
 7. optionally contain extra files, directly included from the cpp file
 8. place any extra files under a directory with the same basename as the cpp file, minus the file extension
 9. be self-contained, not needing externally installed dependencies (besides the locally available extra files defined in the previous point)
10. be contained within DGL_NAMESPACE
11. have clear documentation specifying its use
12. be cross-platform (win32 and posix)
13. be liberally licensed (the same way as DPF or with a compatible license allowing commercial use)

Coding style rules yet to be defined.
