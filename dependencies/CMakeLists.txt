# CMake packages
# provides fmt
cpmaddpackage(
        NAME serious-meta-library
        GIT_REPOSITORY "git@github.com:UsatiyNyan/serious-meta-library.git"
        GIT_TAG 1.0.1)
# provides spdlog
cpmaddpackage(
        NAME serious-runtime-library
        GIT_REPOSITORY "git@github.com:UsatiyNyan/serious-runtime-library.git"
        GIT_TAG main)

include("${CMAKE_CURRENT_SOURCE_DIR}/boost.cmake")

string(TOUPPER "${${PROJECT_NAME}_PROTOCOL}" "${PROJECT_NAME}_PROTOCOL")
if (${PROJECT_NAME}_PROTOCOL STREQUAL "WAYLAND")
    set(${PROJECT_NAME}_BUILD_WAYLAND ON)
    set(${PROJECT_NAME}_BUILD_X11 OFF)
elseif(${PROJECT_NAME}_PROTOCOL STREQUAL "X11")
    set(${PROJECT_NAME}_BUILD_WAYLAND OFF)
    set(${PROJECT_NAME}_BUILD_X11 ON)
elseif(${PROJECT_NAME}_PROTOCOL STREQUAL "ALL")
    set(${PROJECT_NAME}_BUILD_WAYLAND ON)
    set(${PROJECT_NAME}_BUILD_X11 ON)
else()
    message(FATAL_ERROR "${PROJECT_NAME}_PROTOCOL=${${PROJECT_NAME}_PROTOCOL} not one of WAYLAND,X11,ALL")
endif()

cpmaddpackage(
        NAME glfw
        GIT_REPOSITORY "git@github.com:glfw/glfw.git"
        GIT_TAG 3.4
        OPTIONS
        "GLFW_BUILD_WAYLAND ${${PROJECT_NAME}_BUILD_WAYLAND}"
        "GLFW_BUILD_X11 ${${PROJECT_NAME}_BUILD_X11}"
)

cpmaddpackage(
        NAME glm
        GIT_REPOSITORY "git@github.com:g-truc/glm.git"
        GIT_TAG 0.9.9.8)

# Manually compiled
add_subdirectory(glad)
add_subdirectory(stb)
add_subdirectory(imgui)

sl_target_link_system_libraries(${PROJECT_NAME}
        PUBLIC
        # utils
        sl::meta
        sl::rt
        Boost::container

        # graphics
        glfw
        glad
        glm

        # image
        stb_image

        # ui
        imgui
        implot

        PRIVATE
        # dynamic graphics API loading
        ${CMAKE_DL_LIBS}
)

if (${${PROJECT_NAME}_BUILD_WAYLAND})
    find_package(OpenGL REQUIRED COMPONENTS EGL)
    sl_target_link_system_libraries(${PROJECT_NAME}
        PUBLIC
        wayland-client
        xkbcommon
        OpenGL::EGL
        )
endif ()
