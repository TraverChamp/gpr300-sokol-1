#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

//uniforms
uniform sample2D screenTexture;
//consts
const float offset 1.0/3000.0;

const float kernel[9] = float[](
  -1,-1,-1,
  -1,-9,-1,
  -1,-1,-1
)
const vec2 offsets[9] = vec2[](
  vec2(-offset, offset), //top-left
  vec2(0.0f, offset),    //top-center
  vec2(offset, offset),  //top-right
  vec2(-offset, 0.0f),   //center-left
  vec2(0.0f, 0.0f),      //center-center
  vec2(offset, 0.0f),    //center-right
  vec2(-offset, -offset), //bottom-left
  vec2(0.0f, -offset),    //bottom-center
  vec2(offset, -offset)   //bottom-right
)

void main()
{
  FragColor = vec4(color, 1.0);
}