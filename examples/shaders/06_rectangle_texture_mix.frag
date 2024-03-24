#version 460 core

in vec2 tex_coord_msg;

out vec4 frag_color;

uniform sampler2D us_texture_bg;
uniform sampler2D us_texture_fg;

void main() {
    frag_color = mix(texture(us_texture_bg, tex_coord_msg), texture(us_texture_fg, tex_coord_msg), 0.4);
}
