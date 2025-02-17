# CMake packages
cpmaddpackage(
        NAME serious-meta-library
        GIT_REPOSITORY "git@github.com:UsatiyNyan/serious-meta-library.git"
        GIT_TAG main)

cpmaddpackage(
        NAME glfw
        GIT_REPOSITORY "git@github.com:glfw/glfw.git"
        GIT_TAG 3.3.8)
cpmaddpackage(
        NAME glm
        GIT_REPOSITORY "git@github.com:g-truc/glm.git"
        GIT_TAG 0.9.9.8)

cpmaddpackage(
        NAME fmt
        GIT_REPOSITORY "git@github.com:fmtlib/fmt.git"
        GIT_TAG 10.1.1)

include("${CMAKE_CURRENT_SOURCE_DIR}/boost.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/spdlog.cmake")

# Manually compiled
add_subdirectory(glad)
add_subdirectory(stb)
add_subdirectory(imgui)

if (UNIX)
    sl_target_link_system_libraries(${PROJECT_NAME}
        PRIVATE
        # dynamic graphics API loading
        dl
    )
endif()

sl_target_link_system_libraries(${PROJECT_NAME}
        PUBLIC
        # graphics
        glfw
        glad
        glm

        # fmt/log
        fmt::fmt
        spdlog::spdlog

        # fp components
        sl::meta
        range-v3::range-v3

        # utils
        Boost::container

        # image
        stb_image

        # ui
        imgui
        implot
)
