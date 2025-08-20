#version 450 core

layout(location = 0) in vec3 in_vertex;
layout(location = 0) out vec2 out_color;

// layout(binding = 0) uniform BaseProperty {
//     mat4 projection;
//     vec3 position;
// };

void main() {
    // mat4 position_matrix = mat4(
    //     vec4(1, 0, 0, 0),
    //     vec4(0, 1, 0, 0),
    //     vec4(0, 0, 1, 0),
    //     vec4(position.xyz, 1)
    // );

    // gl_Position = projection * vec4(in_vertex, 1.0);
    gl_Position = vec4(in_vertex, 1.0);
    out_color = (gl_Position.xy + vec2(1)) / vec2(2);
}
