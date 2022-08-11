#version 460 core

#define MAX_DIRECTIONAL_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT       8
#define MAX_SPOT_LIGHT_COUNT        8

uniform vec3 VIEW_POS;

uniform struct {
  vec3  color;
  float intensity;
} AMBIENT_LIGHT;
uniform struct {
  vec3  color;
  float intensity;

  vec3 dir;
} DIRECTIONAL_LIGHTS[MAX_DIRECTIONAL_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3  color;
  float intensity;

  float linAttCoef;
  float quadAttCoef;
} POINT_LIGHTS[MAX_POINT_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3  color;
  float intensity;

  vec3 dir;

  float linAttCoef;
  float quadAttCoef;

  float angle;
  float smoothAngle;
} SPOT_LIGHTS[MAX_SPOT_LIGHT_COUNT];

uniform struct {
  float ambCoef;
  float diffCoef;
  float specCoef;

  float glossiness;

  float parallaxStrength;

  sampler2D albedoMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} MATERIAL;

in vec3 fWorldPos;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 FragColor;

void calcLambertianLight(out vec3 diffuse,
                        vec3 N,
                        vec3 L,
                        float attenuation,
                        vec3 color) {
  float LdotN = max(dot(L, N), 0.0f);

  // Calculating diffuse (Lambertian) light
  diffuse = color
          * LdotN
          * attenuation
          * MATERIAL.diffCoef;
}

void calcPhongLight(out vec3 diffuse,
                    out vec3 specular,
                    vec3 N,
                    vec3 L,
                    float attenuation,
                    vec3 color) {
  // Calculating diffuse (Lambertian) light
  calcLambertianLight(diffuse, N, L, attenuation, color);

  vec3 R = reflect(-L, N); // 2.0f * dot(L, N) * N - L;
  vec3 V = normalize(VIEW_POS - fWorldPos);

  float gloss    = MATERIAL.glossiness * (1.0f - texture(MATERIAL.roughMap, fTexCoords).r);
  float glossExp = exp2(gloss);
  float VdotR    = max(dot(V, R), 0.0f);

  // Calculating specular (Phong) light
  specular = color
           * gloss * pow(VdotR, glossExp)
           * attenuation
           * MATERIAL.specCoef;
}

void calcBlinnPhongLight(out vec3 diffuse,
                         out vec3 specular,
                         vec3 N,
                         vec3 L,
                         float attenuation,
                         vec3 color) {
  // Calculating diffuse (Lambertian) light
  calcLambertianLight(diffuse, N, L, attenuation, color);

  float gloss    = MATERIAL.glossiness * (1.0f - texture(MATERIAL.roughMap, fTexCoords).r);
  float glossExp = 2.0f * exp2(gloss);
  vec3  V        = normalize(VIEW_POS - fWorldPos);
  vec3  H        = normalize(L + V);
  float HdotN    = max(dot(H, N), 0.0f);
  float LdotN    = max(dot(L, N), 0.0f);

  // Calculating specular (Blinn-Phong) light
  specular = color
           * gloss * pow(HdotN, glossExp) * float(LdotN > 0.0f)
           * attenuation
           * MATERIAL.specCoef;
}

float calcLightAttenuation(vec3 worldPos, float linAttCoef, float quadAttCoef) {
  float dist = length(worldPos - fWorldPos);

  return 1.0f / (1.0f + linAttCoef * dist + quadAttCoef * dist * dist);
}

void calcDirectionalLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(-DIRECTIONAL_LIGHTS[index].dir);

  // Calculating light distance attenuation
  float attenuation = 1.0f;

  vec3 color = normalize(DIRECTIONAL_LIGHTS[index].color) * DIRECTIONAL_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);
}

void calcPointLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(POINT_LIGHTS[index].worldPos - fWorldPos);

  // Calculating light distance attenuation
  float attenuation = calcLightAttenuation(
      POINT_LIGHTS[index].worldPos,
      POINT_LIGHTS[index].linAttCoef,
      POINT_LIGHTS[index].quadAttCoef
  );

  vec3 color = normalize(POINT_LIGHTS[index].color) * POINT_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);
}

void calcSpotLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(SPOT_LIGHTS[index].worldPos - fWorldPos);

  // Calculating light distance attenuation
  float attenuation = calcLightAttenuation(
      SPOT_LIGHTS[index].worldPos,
      SPOT_LIGHTS[index].linAttCoef,
      SPOT_LIGHTS[index].quadAttCoef
  );
  float angle       = cos(SPOT_LIGHTS[index].angle);
  vec3  D           = normalize(-SPOT_LIGHTS[index].dir);
  float LdotD       = dot(L, D);
  float smoothAngle = cos(SPOT_LIGHTS[index].smoothAngle);
  //attenuation      *= 1.0f - min((1.0f - LdotD) / (1.0f - angle), 1.0f);
  attenuation      *= 1.0f - min((smoothAngle - LdotD) / (smoothAngle - angle), 1.0f);

  vec3 color = normalize(SPOT_LIGHTS[index].color) * SPOT_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);
}

// Fragment shader
void main() {
  // Initializing Phong/Blinn-Phong light model components
  vec3 ambient  = AMBIENT_LIGHT.color * AMBIENT_LIGHT.intensity
                * texture(MATERIAL.ambOccMap, fTexCoords).r
                * MATERIAL.ambCoef;
  vec3 diffuse  = vec3(0.0f);
  vec3 specular = vec3(0.0f);

  // Adding each directional light contribution
  for (uint i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcDirectionalLight(deltaDiffuse, deltaSpecular, fNormal, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each point light contribution
  for (uint i = 0; i < MAX_POINT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcPointLight(deltaDiffuse, deltaSpecular, fNormal, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each spot light contribution
  for (uint i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcSpotLight(deltaDiffuse, deltaSpecular, fNormal, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding together all the light components
  vec3 light = ambient + diffuse + specular;

  // Getting albedo and emission maps texels (UV-mapping)
  vec4 albedoTexel   = texture(MATERIAL.albedoMap, fTexCoords);
  vec4 emissionTexel = texture(MATERIAL.emissMap, fTexCoords);

  // Calculating fragment color by albedo map, light and also emission map
  vec3  color   = albedoTexel.rgb * light + emissionTexel.rgb * emissionTexel.a;

  FragColor = vec4(color, 1.0f);
}
