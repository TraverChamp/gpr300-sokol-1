#version 410

precision mediump float;

out vec4 FragColor;
struct Light {
  vec3 color;
  vec3 pos;
};
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D zaToon;
uniform vec3 camera;
uniform Light light;
uniform Material material;

vec3 highlight = vec3(1.0, 0.0, 1.0);
vec3 shadow = vec3(0.0, 0.0, 1.0);

vec3 toonLighting(vec3 normal, vec3 frag_pos, Light light) {
  vec3 lightDir = normalize(light.pos - frag_pos);
  
  float NDotL = (dot(normal, lightDir)+1.0)/2.0;
  float zaToon = texture(zaToon, vec2(NDotL)).r;
  vec3 color = mix(shadow, highlight, zaToon);
  return color;
}

void main()
{
  vec3 norm = normalize(vs_normal);
  vec3 color = toonLighting(norm, vs_position, light);
  FragColor = vec4(color, 1.0);
}