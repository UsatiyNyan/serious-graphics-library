#version 460 core

out vec4 frag_color;

uniform vec3 u_light_color;

void main() {
    frag_color = vec4(u_light_color, 1.0);
}
