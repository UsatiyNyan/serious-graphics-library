sl_add_example(${PROJECT_NAME} 0_blank_window)

sl_add_example(${PROJECT_NAME} 1_triangle)
sl_target_attach_directory(${PROJECT_NAME}_1_triangle shaders)

sl_add_example(${PROJECT_NAME} 2_rectangle)
sl_target_attach_directory(${PROJECT_NAME}_2_rectangle shaders)

sl_add_example(${PROJECT_NAME} 3_two_triangles)
sl_target_attach_directory(${PROJECT_NAME}_3_two_triangles shaders)

sl_add_example(${PROJECT_NAME} 4_colored_triangle)
sl_target_attach_directory(${PROJECT_NAME}_4_colored_triangle shaders)

sl_add_example(${PROJECT_NAME} 5_textured_triangle)
sl_target_attach_directory(${PROJECT_NAME}_5_textured_triangle shaders)
sl_target_attach_directory(${PROJECT_NAME}_5_textured_triangle textures)

sl_add_example(${PROJECT_NAME} 6_rectangle_texture_mix)
sl_target_attach_directory(${PROJECT_NAME}_6_rectangle_texture_mix shaders)
sl_target_attach_directory(${PROJECT_NAME}_6_rectangle_texture_mix textures)

sl_add_example(${PROJECT_NAME} 7_rectangle_transform)
sl_target_attach_directory(${PROJECT_NAME}_7_rectangle_transform shaders)
sl_target_attach_directory(${PROJECT_NAME}_7_rectangle_transform textures)
