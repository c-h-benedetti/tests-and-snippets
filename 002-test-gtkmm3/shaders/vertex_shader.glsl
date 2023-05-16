#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 uv_position;

out vec3 v_color;

void main() {
    gl_Position = vec4(vertex_position, 1.0);
    v_color = vec3(uv_position, 0.5);
}
