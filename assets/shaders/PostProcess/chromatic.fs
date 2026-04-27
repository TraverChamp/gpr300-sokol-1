#version 410

precision mediump float;

out vec4 FragColor;

// varyings

in vec2 vs_texcoord;

//uniforms
uniform sampler2D screenTexture;
//const
const vec3 offset    = vec3(0.019, 0.002, 0.035);
const vec2 direction = vec2(1.0);

void main()
{
  FragColor.r = texture(screenTexture, vs_texcoord + (direction * vec2(offset.r))).r;
  FragColor.g = texture(screenTexture, vs_texcoord + (direction * vec2(offset.g))).g;
  FragColor.b = texture(screenTexture, vs_texcoord + (direction * vec2(offset.b))).b;
}