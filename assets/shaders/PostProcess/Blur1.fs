#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

//uniforms
uniform sampler2D screenTexture;
//consts
const float offset = 1.0/300.0;

    float kernel[9] = float[](
         1.0/16.0, 2.0/16.0, 1.0/16.0,
         2.0/16.0, 4.0/16.0, 2.0/16.0,
         1.0/16.0, 2.0/16.0, 1.0/16.0 
    );
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
);

void main()
{
  vec3 sampleTex[9];
  for(int i=0; i<9; i++) {
    sampleTex[i] = vec3(texture(screenTexture, vs_texcoord.xy + offsets[i]));
  }
  vec3 col = vec3(0.0);
  for(int i = 0;i<9;i++) {
    col += sampleTex[i] * kernel[i];
  }
  FragColor = vec4(col, 1.0);
}