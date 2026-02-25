#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;
uniform sampler2D screenTexture;

void main()
{
  FragColor = vec4(vec3(1.0-texture(screenTexture, vs_texcoord)), 1.0);
}