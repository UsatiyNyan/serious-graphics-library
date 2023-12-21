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
cpmaddpackage(
        NAME spdlog
        GIT_REPOSITORY "git@github.com:gabime/spdlog.git"
        GIT_TAG v1.12.0)

cpmaddpackage(
        NAME libassert
        GIT_REPOSITORY "git@github.com:jeremy-rifkin/libassert.git"
        GIT_TAG main)
cpmaddpackage(
        NAME function2
        GIT_REPOSITORY "git@github.com:Naios/function2.git"
        GIT_TAG 4.2.4)
cpmaddpackage(
        NAME range-v3
        GIT_REPOSITORY "git@github.com:ericniebler/range-v3.git"
        GIT_TAG 0.12.0)

include("${CMAKE_CURRENT_SOURCE_DIR}/tl.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/boost.cmake")

# Manually compiled
add_subdirectory(glad)
add_subdirectory(stb)

sl_target_link_system_libraries(${PROJECT_NAME}
        PRIVATE
        # dynamic graphics API loading
        dl

        PUBLIC
        # graphics
        glfw
        glad
        glm

        # fmt/log
        fmt::fmt
        spdlog::spdlog

        # fp components
        serious-meta-library
        tl::optional
        tl::expected
        function2::function2
        range-v3::range-v3

        # utils
        assert
        Boost::container

        # image
        stb_image
)
