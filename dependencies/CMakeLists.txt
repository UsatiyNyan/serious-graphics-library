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
cpmaddpackage(
        NAME spdlog
        GIT_REPOSITORY "git@github.com:gabime/spdlog.git"
        GIT_TAG v1.12.0)
cpmaddpackage(
        NAME tl-optional
        GIT_REPOSITORY "git@github.com:TartanLlama/optional.git"
        GIT_TAG v1.1.0)
cpmaddpackage(
        NAME tl-expected
        GIT_REPOSITORY "git@github.com:TartanLlama/expected.git"
        GIT_TAG v1.1.0)
cpmaddpackage(
        NAME libassert
        GIT_REPOSITORY "git@github.com:jeremy-rifkin/libassert.git"
        GIT_TAG main)
cpmaddpackage(
        NAME function2
        GIT_REPOSITORY "git@github.com:Naios/function2.git"
        GIT_TAG 4.2.4)

sl_target_link_system_libraries(${PROJECT_NAME}
        PRIVATE
        # dynamic graphics API loading
        dl

        PUBLIC
        # graphics
        glfw
        glad

        # fmt/log
        fmt
        spdlog

        # fp components
        tl::optional
        tl::expected
        function2

        # utils
        assert
)
