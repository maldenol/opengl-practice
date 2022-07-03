#version 460 core

const uint MAX_DIRECTIONAL_LIGHT_COUNT = 8;
const uint MAX_POINT_LIGHT_COUNT       = 8;
const uint MAX_SPOT_LIGHT_COUNT        = 8;

uniform vec3 viewPos;
uniform mat4 model;

uniform vec3  ambLightColor;
uniform float ambLightIntensity;
uniform struct {
  vec3  color;
  float intensity;

  vec3 dir;
} directionalLights[MAX_DIRECTIONAL_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3  color;
  float intensity;

  float linAttCoef;
  float quadAttCoef;
} pointLights[MAX_POINT_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3  color;
  float intensity;

  vec3 dir;

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

  sampler2D albedoMap;
  sampler2D normalMap;
  sampler2D heightMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} material;

in vec3 fWorldPos;
in vec3 fNormal;
in mat3 fTBN;
in vec2 fTexCoords;

out vec4 FragColor;

void calcPhongLight(out vec3 diffuse,
                    out vec3 specular,
                    vec3 N,
                    vec3 L,
                    float attenuation,
                    vec3 color) {
  float LdotN = max(dot(L, N), 0.0f);

  diffuse = color * LdotN * material.diffCoef * attenuation;

  vec3 R = reflect(-L, N); // 2.0f * dot(L, N) * N - L;
  vec3 V = normalize(viewPos - fWorldPos);

  float gloss    = material.glossiness * (1.0f - texture(material.roughMap, fTexCoords).r);
  float glossExp = exp2(gloss);
  float VdotR    = max(dot(V, R), 0.0f);

  specular = color
           * gloss * pow(VdotR, glossExp)
           * attenuation
           * material.specCoef;
}

void calcBlinnPhongLight(out vec3 diffuse,
                         out vec3 specular,
                         vec3 N,
                         vec3 L,
                         float attenuation,
                         vec3 color) {
  float LdotN = max(dot(L, N), 0.0f);

  diffuse = color * LdotN * material.diffCoef * attenuation;

  float gloss    = material.glossiness * (1.0f - texture(material.roughMap, fTexCoords).r);
  float glossExp = 2.0f * exp2(gloss);
  vec3  V        = normalize(viewPos - fWorldPos);
  vec3  H        = normalize(L + V);
  float HdotN    = max(dot(H, N), 0.0f);

  specular = color
           * gloss * pow(HdotN, glossExp) * float(LdotN > 0.0f)
           * attenuation
           * material.specCoef;
}

float calcLightAttenuation(vec3 worldPos, float linAttCoef, float quadAttCoef) {
  float dist = length(worldPos - fWorldPos);

  return 1.0f / (1.0f + linAttCoef * dist + quadAttCoef * dist * dist);
}

void calcDirectionalLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(-directionalLights[index].dir);

  float attenuation = 1.0f;

  vec3 color = normalize(directionalLights[index].color) * directionalLights[index].intensity;

  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);
}

void calcPointLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(pointLights[index].worldPos - fWorldPos);

  float attenuation = calcLightAttenuation(
      pointLights[index].worldPos,
      pointLights[index].linAttCoef,
      pointLights[index].quadAttCoef
  );

  vec3 color = normalize(pointLights[index].color) * pointLights[index].intensity;

  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);
}

void calcSpotLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(spotLights[index].worldPos - fWorldPos);

  float attenuation = calcLightAttenuation(
      spotLights[index].worldPos,
      spotLights[index].linAttCoef,
      spotLights[index].quadAttCoef
  );
  float angle        = cos(spotLights[index].angle);
  vec3  D            = normalize(-spotLights[index].dir);
  float LdotD        = dot(L, D);
  float smoothAngle  = cos(spotLights[index].smoothAngle);
  //attenuation       *= 1.0f - min((1.0f - LdotD) / (1.0f - angle), 1.0f);
  attenuation       *= 1.0f - min((smoothAngle - LdotD) / (smoothAngle - angle), 1.0f);

  vec3 color = normalize(spotLights[index].color) * spotLights[index].intensity;

  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);
}

// Fragment shader
void main() {
  // Initializing Phong/Blinn-Phong light model components
  vec3 ambient  = ambLightColor * ambLightIntensity
                * texture(material.ambOccMap, fTexCoords).r
                * material.ambCoef;
  vec3 diffuse  = vec3(0.0f);
  vec3 specular = vec3(0.0f);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(fTBN * (vec3(texture(material.normalMap, fTexCoords)) * 2.0f - 1.0f));
  //vec3 N = mat3(transpose(inverse(model))) * fNormal;

  // Adding each directional light contribution
  for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcDirectionalLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each point light contribution
  for (int i = 0; i < MAX_POINT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcPointLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each spot light contribution
  for (int i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcSpotLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Add together all light components
  vec3 light = ambient + diffuse + specular;

  // Calculating fragment color by albedo map, color and also emission map
  FragColor = texture(material.albedoMap, fTexCoords) * vec4(light, 1.0f)
            + texture(material.emissMap, fTexCoords);
}
