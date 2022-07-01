#version 460 core

uniform vec3 viewPos;
uniform mat4 model;

uniform vec3 ambLightColor;
uniform struct {
  vec3 worldPos;
  vec3 color;
} light;

uniform struct {
  float ambCoef;
  float diffCoef;
  float specCoef;
  float glossiness;

  sampler2D texture;
  sampler2D normalMap;
  sampler2D heightMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} material;

in vec3 fWorldPos;
in vec3 fNormal;
in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragColor;

void main() {
  vec3 ambient = ambLightColor * texture(material.ambOccMap, fTexCoords).r * material.ambCoef;

  vec3 N = mat3(transpose(inverse(model))) * fNormal;
  vec3 L = normalize(light.worldPos - fWorldPos);

  float LdotN   = max(dot(L, N), 0.0f);
  vec3  diffuse = light.color * LdotN * material.diffCoef;

  vec3 R = reflect(-L, N);
  vec3 V = normalize(viewPos - fWorldPos);

  float gloss    = material.glossiness * (1.0f - texture(material.roughMap, fTexCoords).r);
  float glossExp = exp2(gloss);
  float VdotR    = max(dot(V, R), 0.0f);
  vec3  specular = light.color * gloss * pow(VdotR, glossExp) * material.specCoef;

  vec3 phong = fColor * (ambient + diffuse + specular);

  FragColor = texture(material.texture, fTexCoords) * vec4(phong, 1.0f) + texture(material.emissMap, fTexCoords);
}
