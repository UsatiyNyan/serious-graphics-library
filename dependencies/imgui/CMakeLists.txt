cpmaddpackage(
        NAME imgui
        DOWNLOAD_ONLY
        GIT_REPOSITORY "git@github.com:ocornut/imgui.git"
        GIT_TAG v1.90
)
add_library(imgui STATIC
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
)
target_include_directories(imgui
        PUBLIC "${imgui_SOURCE_DIR}"
)
sl_target_link_system_libraries(imgui
        PUBLIC
        glfw
        glad
)

# force include, otherwise GLFW3 doesn't configure
if (MSVC)
    target_compile_options(imgui PRIVATE /FI"glad/glad.h" )
else () # GCC or Clang
    target_compile_options(imgui PRIVATE -include glad/glad.h)
endif ()

target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM)
