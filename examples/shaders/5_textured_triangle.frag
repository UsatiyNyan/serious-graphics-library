#version 460 core

in vec2 tex_coord_msg;

out vec4 frag_color;

uniform sampler2D us_texture;

void main() {
    frag_color = texture(us_texture, tex_coord_msg);
}
