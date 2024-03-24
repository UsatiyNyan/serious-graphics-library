#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_color;

out vec3 color_msg;

void main() {
    gl_Position = vec4(in_pos, 1.0);
    color_msg = in_color;
}
