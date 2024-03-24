#version 460 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coord;

out vec2 tex_coord_msg;

uniform mat4 u_transform;

void main() {
    gl_Position = u_transform * vec4(in_pos, 1.0);
    tex_coord_msg = in_tex_coord;
}
