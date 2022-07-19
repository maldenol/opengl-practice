#version 460 core

#define MAX_DIRECTIONAL_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT       1
#define MAX_SPOT_LIGHT_COUNT        8

const float kKernelOffset = 0.001f;
const vec2 kKernelOffsets[] = {
  vec2(-kKernelOffset, kKernelOffset),
  vec2(0.0f, kKernelOffset),
  vec2(kKernelOffset, kKernelOffset),
  vec2(-kKernelOffset, 0.0f),
  vec2(0.0f, 0.0f),
  vec2(kKernelOffset, 0.0f),
  vec2(-kKernelOffset, -kKernelOffset),
  vec2(0.0f, -kKernelOffset),
  vec2(kKernelOffset, -kKernelOffset),
};

uniform vec3 VIEW_POS;

uniform struct {
  vec3  color;
  float intensity;
} AMBIENT_LIGHT;
uniform struct {
  vec3  color;
  float intensity;

  vec3 dir;

  sampler2D shadowMap;
  mat4      VP;
} DIRECTIONAL_LIGHTS[MAX_DIRECTIONAL_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3  color;
  float intensity;

  float linAttCoef;
  float quadAttCoef;

  samplerCube shadowMap;
  float       farPlane;
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

  sampler2D shadowMap;
  mat4      VP;
} SPOT_LIGHTS[MAX_SPOT_LIGHT_COUNT];

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
} MATERIAL;

in Interpolators {
  vec3 worldPos;
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} i;

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
  vec3 V = normalize(VIEW_POS - i.worldPos);

  float gloss    = MATERIAL.glossiness * (1.0f - texture(MATERIAL.roughMap, i.texCoords).r);
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

  float gloss    = MATERIAL.glossiness * (1.0f - texture(MATERIAL.roughMap, i.texCoords).r);
  float glossExp = 2.0f * exp2(gloss);
  vec3  V        = normalize(VIEW_POS - i.worldPos);
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
  float dist = length(worldPos - i.worldPos);

  return 1.0f / (1.0f + linAttCoef * dist + quadAttCoef * dist * dist);
}

void calcDirectionalLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(-DIRECTIONAL_LIGHTS[index].dir);

  float attenuation = 1.0f;

  vec3 color = normalize(DIRECTIONAL_LIGHTS[index].color) * DIRECTIONAL_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);

  // Calculating fragment coordinates in light space
  vec4 lightSpaceFragCoords = DIRECTIONAL_LIGHTS[index].VP * vec4(i.worldPos, 1.0f);
  lightSpaceFragCoords     /= lightSpaceFragCoords.w;
  lightSpaceFragCoords      = lightSpaceFragCoords * 0.5f + vec4(vec3(0.5f), 0.0f);

  // Calculating shadow coefficient (Percentage-Closer Filtering)
  float notInShadow = 0.0f;
  for (uint i = 0; i < 9; ++i) {
    // Calculating fragment and obstacle depth
    float fragmentDepth = lightSpaceFragCoords.z;
    float obstacleDepth = texture(
        DIRECTIONAL_LIGHTS[index].shadowMap,
        lightSpaceFragCoords.xy + kKernelOffsets[i]
    ).r;

    // Applying shadow bias
    float LdotN    = max(dot(L, N), 0.0f);
    float bias     = max(0.05f * (1.0f - LdotN), 0.001f);
    obstacleDepth += bias;

    // Calculating if fragment is not in shadow
    notInShadow += float(fragmentDepth <= obstacleDepth || fragmentDepth >= 1.0f);
  }
  notInShadow /= 9.0f;

  // Applying shadow
  diffuse  *= notInShadow;
  specular *= notInShadow;
}

void calcPointLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(POINT_LIGHTS[index].worldPos - i.worldPos);

  // Calculating light distance attenuation
  float attenuation = calcLightAttenuation(
      POINT_LIGHTS[index].worldPos,
      POINT_LIGHTS[index].linAttCoef,
      POINT_LIGHTS[index].quadAttCoef
  );

  vec3 color = normalize(POINT_LIGHTS[index].color) * POINT_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color);

  // Calculating light to fragment vector
  vec3 fragmentToLight = POINT_LIGHTS[index].worldPos - i.worldPos;
  // Fixing negative X-axis
  fragmentToLight.x *= -1.0f;

  // Calculating shadow coefficient (Percentage-Closer Filtering)
  float notInShadow = 0.0f;
  for (uint i = 0; i < 9; ++i) {
    // Calculating fragment and obstacle depth
    float fragmentDepth = length(fragmentToLight) / POINT_LIGHTS[index].farPlane;
    float obstacleDepth = texture(
        POINT_LIGHTS[index].shadowMap,
        fragmentToLight
    ).r;

    // Applying shadow bias
    float LdotN    = max(dot(L, N), 0.0f);
    float bias     = max(0.05f * (1.0f - LdotN), 0.001f);
    obstacleDepth += bias;

    // Calculating if fragment is not in shadow
    notInShadow += float(fragmentDepth <= obstacleDepth || fragmentDepth >= 1.0f);
  }
  notInShadow /= 9.0f;

  // Applying shadow
  diffuse  *= notInShadow;
  specular *= notInShadow;
}

void calcSpotLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index) {
  vec3 L = normalize(SPOT_LIGHTS[index].worldPos - i.worldPos);

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

  // Calculating fragment coordinates in light space
  vec4 lightSpaceFragCoords = SPOT_LIGHTS[index].VP * vec4(i.worldPos, 1.0f);
  lightSpaceFragCoords     /= lightSpaceFragCoords.w;
  lightSpaceFragCoords      = lightSpaceFragCoords * 0.5f + vec4(vec3(0.5f), 0.0f);

  // Calculating shadow coefficient (Percentage-Closer Filtering)
  float notInShadow = 0.0f;
  for (uint i = 0; i < 9; ++i) {
    // Calculating fragment and obstacle depth
    float fragmentDepth = lightSpaceFragCoords.z;
    float obstacleDepth = texture(
        SPOT_LIGHTS[index].shadowMap,
        lightSpaceFragCoords.xy + kKernelOffsets[i]
    ).r;

    // Applying shadow bias
    float LdotN    = max(dot(L, N), 0.0f);
    float bias     = max(0.05f * (1.0f - LdotN), 0.001f);
    obstacleDepth += bias;

    // Calculating if fragment is not in shadow
    notInShadow += float(fragmentDepth <= obstacleDepth || fragmentDepth >= 1.0f);
  }
  notInShadow /= 9.0f;

  // Applying shadow
  diffuse  *= notInShadow;
  specular *= notInShadow;
}

// Fragment shader
void main() {
  // Initializing Phong/Blinn-Phong light model components
  vec3 ambient  = AMBIENT_LIGHT.color * AMBIENT_LIGHT.intensity
                * texture(MATERIAL.ambOccMap, i.texCoords).r
                * MATERIAL.ambCoef;
  vec3 diffuse  = vec3(0.0f);
  vec3 specular = vec3(0.0f);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(i.TBN * (vec3(texture(MATERIAL.normalMap, i.texCoords)) * 2.0f - 1.0f));

  // Adding each directional light contribution
  for (uint i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcDirectionalLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each point light contribution
  for (uint i = 0; i < MAX_POINT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcPointLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each spot light contribution
  for (uint i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcSpotLight(deltaDiffuse, deltaSpecular, N, i);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Add together all light components
  vec3 light = ambient + diffuse + specular;

  // Calculating fragment color by albedo map, color and also emission map
  FragColor = texture(MATERIAL.albedoMap, i.texCoords) * vec4(light, 1.0f)
            + texture(MATERIAL.emissMap, i.texCoords);
}
