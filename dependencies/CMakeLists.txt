# CMake packages
# provides fmt
cpmaddpackage(
        NAME serious-meta-library
        GIT_REPOSITORY "git@github.com:UsatiyNyan/serious-meta-library.git"
        GIT_TAG main)
cpmaddpackage(
        NAME spdlog
        GIT_REPOSITORY "git@github.com:gabime/spdlog.git"
        GIT_TAG v1.12.0
        OPTIONS
        "SPDLOG_FMT_EXTERNAL ON"
        )
include("${CMAKE_CURRENT_SOURCE_DIR}/boost.cmake")

cpmaddpackage(
        NAME glfw
        GIT_REPOSITORY "git@github.com:glfw/glfw.git"
        GIT_TAG 3.3.8)
cpmaddpackage(
        NAME glm
        GIT_REPOSITORY "git@github.com:g-truc/glm.git"
        GIT_TAG 0.9.9.8)

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
        # utils
        sl::meta
        spdlog::spdlog
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
)
