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
in vec4 vs_light_proj_pos;


uniform sampler2D shadow_map;
uniform vec3 camera;
uniform Light light;
uniform Material material;


float shadowCalc(vec4 fragposLightSpace) {
  float shadow = 0.255;
  //divide perspective
  vec3 proj_coords = fragposLightSpace.xyz / fragposLightSpace.w;

  proj_coords = proj_coords * 0.5 + 0.5;

  float closest_depth = texture(shadow_map, proj_coords.xy).r;
  float current_depth = proj_coords.z;
  shadow = (current_depth > closest_depth) ? 1.0 : 0;
  return shadow;
}
vec3 blinnPhong(vec3 normal, vec3 frag_pos, Light light) {

//float diffuse = max(dot(normal, light_direction), 0.0);
//float specular = pow(max(dot(normal, halfway_direction), 0.0), material.shininess);

  vec3 viewDir = normalize(camera - frag_pos);
  vec3 lightDir = normalize(light.pos - frag_pos);
  vec3 halfDir = normalize(lightDir + viewDir);

  float NDotL = max(dot(normal, lightDir), 0.0);
  float NDotH = max(dot(normal, halfDir), 0.0);
  
  vec3 diffuse = NDotL * material.diffuse;
  vec3 specular = pow(NDotH, material.shininess *128.0) * material.specular;

  return (diffuse + specular) * light.color;
}

void main()
{
  vec3 norm = normalize(vs_normal);
  float shadow = shadowCalc(vs_light_proj_pos);
  vec3 color = blinnPhong(norm, vs_position, light);
  
  color *= (1.0 - shadow);
  color += material.ambient;
  color*= light.color;
 
  vec3 objColor = norm * 0.5 + 0.5;
  vec3 finalC = color*objColor;
  FragColor = vec4(finalC, 1.0);
}