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
  vec3 shininess;
};
// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform vec3 camera;
uniform float alpha;
uniform Light light;
uniform Material material;

vec3 effect(vec3 normal, vec3 frag_pos, Light light) {
  vec3 viewDir = normalize(camera - frag_pos);
  vec3 lightDir = normalize(light.pos - frag_pos);
  vec3 halfDir = normalize(viewDir + lightDir);

  float NDotL = max(dot(normal, lightDir), 0.0);
  float NDotH = max(dot(normal, halfDir), 0.0);
  
  vec3 diffuse = NDotL * material.diffuse;
  vec3 specular = NDotH * material.specular;

  vec3 lightCalc = vec3(diffuse) + vec3(pow(specular, material.shininess));

  float pDotL = dot(frag_pos.xyz, light.pos.xyz);
  return lightCalc * light.color;
}

void main()
{
  vec3 color = effect(vs_normal, vs_position, light);
  vec3 objColor = vs_normal * 0.5 + 0.5;
  vec3 finalC = objColor*color + material.ambient;
  FragColor = vec4(finalC, 1.0);
}