#version 460 core

const uint MAX_DIRECTIONAL_LIGHT_COUNT = 8;
const uint MAX_POINT_LIGHT_COUNT       = 8;
const uint MAX_SPOT_LIGHT_COUNT        = 8;

uniform vec3 viewPos;
uniform mat4 model;

uniform vec3 ambLightColor;
uniform struct {
  vec3 color;

  vec3 dir;
} directionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
uniform struct {
  vec3  worldPos;

  vec3  color;

  float linAttCoef;
  float quadAttCoef;
} pointLights[MAX_POINT_LIGHT_COUNT];
uniform struct {
  vec3  worldPos;

  vec3  color;

  vec3  dir;

  float linAttCoef;
  float quadAttCoef;

  float angle;
  float smoothAngle;
} spotLights[MAX_SPOT_LIGHT_COUNT];

uniform struct {
  float ambCoef;
  float diffCoef;
  float specCoef;

  float glossiness;

  float maxHeight;

  sampler2D texture;
  sampler2D normalMap;
  sampler2D heightMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} material;

in vec3 fWorldPos;
in vec3 fNormal;
in vec3 fTangent;
in vec2 fTexCoords;

out vec4 FragColor;

void calcLight(out vec3 diffuse, out vec3 specular, vec3 N, vec3 L, float attenuation, vec3 color) {
  float LdotN = max(dot(L, N), 0.0f);

  diffuse = color * LdotN * material.diffCoef * (1.0f - attenuation);

  vec3 R = reflect(-L, N); // 2.0f * dot(L, N) * N - L;
  vec3 V = normalize(viewPos - fWorldPos);

  float gloss    = material.glossiness * (1.0f - texture(material.roughMap, fTexCoords).r);
  float glossExp = exp2(gloss);
  float VdotR    = max(dot(V, R), 0.0f);

  specular = color * gloss * pow(VdotR, glossExp) * material.specCoef * (1.0f - attenuation);
}

float calcLightAttenuation(vec3 worldPos, float linAttCoef, float quadAttCoef) {
  float dist = length(worldPos - fWorldPos);

  return 1.0f / (1.0f + linAttCoef * dist + quadAttCoef * dist * dist);
}

void calcDirectionalLight(inout vec3 diffuse, inout vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(-directionalLights[index].dir);

  float attenuation = 0.0f;

  calcLight(diffuse, specular, N, L, attenuation, directionalLights[index].color);
}

void calcPointLight(inout vec3 diffuse, inout vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(pointLights[index].worldPos - fWorldPos);

  float attenuation = calcLightAttenuation(
      pointLights[index].worldPos,
      pointLights[index].linAttCoef,
      pointLights[index].quadAttCoef
  );

  calcLight(diffuse, specular, N, L, attenuation, pointLights[index].color);
}

void calcSpotLight(inout vec3 diffuse, inout vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(spotLights[index].worldPos - fWorldPos);

  float dist = length(spotLights[index].worldPos - fWorldPos);
  float attenuation = calcLightAttenuation(
      spotLights[index].worldPos,
      spotLights[index].linAttCoef,
      spotLights[index].quadAttCoef
  );

  float angle        = cos(spotLights[index].angle);
  vec3  D            = normalize(-spotLights[index].dir);
  float LdotD        = dot(L, D);
  float smoothAngle  = cos(spotLights[index].smoothAngle);
  //attenuation       *= min((1.0f - LdotD) / (1.0f - angle), 1.0f);
  attenuation       *= min((smoothAngle - LdotD) / (smoothAngle - angle), 1.0f);

  calcLight(diffuse, specular, N, L, attenuation, spotLights[index].color);
}

void main() {
  vec3 ambient = ambLightColor * texture(material.ambOccMap, fTexCoords).r * material.ambCoef;

  vec3 normal    = normalize(vec3(model * vec4(fNormal, 0.0f)));
  vec3 tangent   = normalize(vec3(model * vec4(fTangent, 0.0f)));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  vec3 N = normalize(TBN * (vec3(texture(material.normalMap, fTexCoords)) * 2.0f - 1.0f));
  //vec3 N = mat3(transpose(inverse(model))) * fNormal;

  vec3 diffuse  = vec3(0.0f);
  vec3 specular = vec3(0.0f);

  for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f), deltaSpecular = vec3(0.0f);
    calcDirectionalLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, 0.0f);
    specular += max(deltaSpecular, 0.0f);
  }

  for (int i = 0; i < MAX_POINT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f), deltaSpecular = vec3(0.0f);
    calcPointLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, 0.0f);
    specular += max(deltaSpecular, 0.0f);
  }

  for (int i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f), deltaSpecular = vec3(0.0f);
    calcSpotLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, 0.0f);
    specular += max(deltaSpecular, 0.0f);
  }

  vec3 phong = ambient + diffuse + specular;

  FragColor = texture(material.texture, fTexCoords) * vec4(phong, 1.0f) + texture(material.emissMap, fTexCoords);
}
