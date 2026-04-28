#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// uniforms
uniform sampler2D screen;
uniform sampler2D lighting;
void main()
{
  vec3 color = texture(lighting, vs_texcoord).rgb;
  vec3 albedo = texture(screen, vs_texcoord).rgb;
  FragColor = vec4(color * albedo, 1.0);
}