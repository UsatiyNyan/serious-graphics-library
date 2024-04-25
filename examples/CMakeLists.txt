cpmaddpackage(
        NAME serious-runtime-library
        GIT_REPOSITORY "git@github.com:UsatiyNyan/serious-runtime-library.git"
        GIT_TAG main)

sl_target_link_system_libraries(${PROJECT_NAME}
        PUBLIC
        serious-runtime-library)

sl_target_attach_directory(${PROJECT_NAME} shaders)
sl_target_attach_directory(${PROJECT_NAME} textures)

sl_add_example(${PROJECT_NAME} 00_blank_window)
sl_add_example(${PROJECT_NAME} 01_triangle)
sl_add_example(${PROJECT_NAME} 02_rectangle)
sl_add_example(${PROJECT_NAME} 03_two_triangles)
sl_add_example(${PROJECT_NAME} 04_colored_triangle)
sl_add_example(${PROJECT_NAME} 05_textured_triangle)
sl_add_example(${PROJECT_NAME} 06_rectangle_texture_mix)
sl_add_example(${PROJECT_NAME} 07_rectangle_transform)
sl_add_example(${PROJECT_NAME} 08_imgui)
sl_add_example(${PROJECT_NAME} 09_implot)
sl_add_example(${PROJECT_NAME} 10_ssbo)
sl_add_example(${PROJECT_NAME} 11_cube)
sl_add_example(${PROJECT_NAME} 12_cubes)
sl_add_example(${PROJECT_NAME} 13_lighting_color)
