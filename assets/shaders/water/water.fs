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


uniform vec3 camera;
uniform Light light;
uniform Material material;
uniform float time;
uniform vec3 waterColor;

void main()
{
  //vec3 norm = normalize(vs_normal);
  //vec3 color = blinnPhong(norm, vs_position, light);
  vec2 dir = vec2(1.0, 0.0);
  vec2 uv = vs_texcoord + vec2(time * dir);
  vec3 objColor = texture(texture0, uv).rgb;
  //vec3 finalC = color*objColor;
  FragColor = vec4(objColor, 1.0);
}