# DPF Widgets - Official and Community made reusable GUI widgets for DPF

This is a repository for shared GUI widgets targetting DPF.
Very much work in progress! Do not use right now unless you plan to contribute some yourself.

Since DPF allows multiple backends (cairo, opengl or vulkan), we need to split them as such.
Each folder provides widgets for its dedicated backend type.
It is not mandatory that a widget is usable for more than 1 backend.
Generic widgets (those based on DPF core classes like Color, Rectangle, etc) are placed under the "generic" directory.

## Rules and Layout

Each widget MUST follow these rules:

1. contain exactly 1 header file, with hpp extension
2. contain 0 or 1 implementation files, with cpp extension and same basename as the matching hpp file
3. optionally contain extra files, directly included from the cpp file
4. place any extra files under a directory with the same basename as the cpp file, minus the file extension
5. be self-contained, not needing externally installed dependencies (besides the locally available extra files defined in point 4)
6. have clear documentation specifying its use
7. be cross-platform (win32 and posix)
8. be liberally licensed (the same way as DPF or with a compatible license allowing commercial use)
