#version 450 core

layout(location = 0) in vec2 in_color;
layout(location = 0) out vec4 out_pixel;

void main() {
    out_pixel = vec4(in_color, 0.0, 1.0);
}
