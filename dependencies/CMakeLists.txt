# Manually compiled

add_subdirectory(glad)

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
        NAME fmt
        GIT_REPOSITORY "git@github.com:fmtlib/fmt.git"
        GIT_TAG 10.1.1)

sl_target_link_system_library(${PROJECT_NAME}
        PRIVATE
        # graphics
        dl # for dynamic graphics API loading
        glfw
        glad

        # utils
        fmt
)
