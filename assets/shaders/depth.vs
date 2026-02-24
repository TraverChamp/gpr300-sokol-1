#version 410
layout(location = 0) in vec3 in_position;

uniform mat4 model;
uniform mat4 light_view_proj

void main() {
    vec3 worldPosition = model * vec4(in_position, 1);
    gl_Position = light_view_proj * world_position;
}