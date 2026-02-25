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
  FragColor = texture(screenTexture, vs_texcoord);
  float avg = (FragColor.r+FragColor.g+FragColor.b)/3.0;
  FragColor = vec4(avg, avg, avg, 1.0);
}