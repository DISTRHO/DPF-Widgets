cmake_minimum_required(VERSION 3.12.4)
project(lvgl-dpf-tests)
set(CMAKE_CXX_STANDARD 11)

set(LV_CONF_PATH "${PROJECT_SOURCE_DIR}/lv_conf.h")

add_subdirectory(${PROJECT_SOURCE_DIR}/../../../DPF ${PROJECT_SOURCE_DIR}/dpf-bin)
add_subdirectory(lvgl ${PROJECT_SOURCE_DIR}/lvgl-bin)

dpf__add_dgl_opengl(FALSE)

add_executable(lvgl-dpf-tests)

target_link_libraries(lvgl-dpf-tests
    PRIVATE
        dgl-opengl
        lvgl::demos
        lvgl::lvgl
)

target_sources(lvgl-dpf-tests
    PRIVATE
        lvgl.cpp
        ${PROJECT_SOURCE_DIR}/../../opengl/LVGL.cpp
)
