cmake_minimum_required(VERSION 3.21)

include(FetchContent)
FetchContent_Declare(
        serious-cmake-library
        GIT_REPOSITORY git@github.com:UsatiyNyan/serious-cmake-library.git
        GIT_TAG main
)
FetchContent_MakeAvailable(serious-cmake-library)

# Project

sl_project_prologue(
        CXX_STANDARD 20
        C_STANDARD 17)

project(
        serious-graphics-library
        VERSION 0.0.1
        DESCRIPTION "graphics library"
        HOMEPAGE_URL "TODO"
        LANGUAGES C CXX)

add_library(${PROJECT_NAME}
        STATIC
        src/ctx/context.cpp
        src/ctx/window.cpp
        src/ctx/imgui.cpp
        src/ctx/implot.cpp
        src/detail/log.cpp
        src/shader/shader.cpp
        src/shader/program.cpp
        src/vtx/vertex_array.cpp
        src/vtx/texture.cpp)
add_library(sl::gfx ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(dependencies)

# Tests and examples

if (NOT PROJECT_IS_TOP_LEVEL)
    return()
endif ()

sl_compiler_warnings(${PROJECT_NAME} WARNINGS_AS_ERRORS)

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()

add_subdirectory(examples)
