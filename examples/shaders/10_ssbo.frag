#version 460 core

out vec4 frag_color;

uniform vec2 u_window_size;

layout (std430, binding = 0) readonly buffer ssbo {
    float data[1024];
};

vec4 draw_quad(vec2 uv, float half_size) {
    if (abs(uv.x) < half_size && abs(uv.y) < half_size) {
        return vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        return vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

vec4 draw_circle(vec2 uv, float half_size) {
    if (length(uv) < half_size) {
        return vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
        return vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

vec4 draw_circle_ssbo(vec2 uv, float radius) {
    const uint index = uint(length(uv / radius) * 1024);
    const float color_coef = index < 1024 ? ((data[index] + 1.0f) / 2) : 0.0f;
    const vec3 color_a = vec3(1.0f, 0.0f, 1.0f);
    const vec3 color_b = vec3(0.0f, 1.0f, 1.0f);
    const float alpha = 1.0f;
    return vec4(mix(color_a, color_b, color_coef), alpha);
}

void main() {
    const vec2 normalized_coord = gl_FragCoord.xy / u_window_size;
    const vec2 aspect_ratio = (u_window_size.x < u_window_size.y)
    ? vec2(1.0f, u_window_size.y / u_window_size.x)
    : vec2(u_window_size.x / u_window_size.y, 1.0f);
    const vec2 uv = (normalized_coord * 2.0f - 1.0f) * aspect_ratio;
    frag_color = draw_circle_ssbo(uv, 2.0f);
}
