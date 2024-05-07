# LVGL for DPF
# Copyright (C) 2024 Filipe Coelho <falktx@falktx.com>
# SPDX-License-Identifier: ISC

add_library(dpf-widgets-lvgl STATIC)
add_library(dpf-widgets::lvgl ALIAS dpf-widgets-lvgl)

target_sources(dpf-widgets-lvgl
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/LVGL.cpp
)

target_include_directories(dpf-widgets-lvgl
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${DPF_ROOT_DIR}/dgl
)

target_link_libraries(dpf-widgets-lvgl
    PUBLIC
        $<$<BOOL:${WIN32}>:winmm>
)
