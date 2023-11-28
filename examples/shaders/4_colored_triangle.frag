#version 460 core

in vec3 color_msg;

out vec4 frag_color;

uniform float u_alpha;

void main() {
    frag_color = vec4(color_msg * u_alpha, 1.0);
}
