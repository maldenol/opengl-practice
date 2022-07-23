#version 460 core

#define MAX_DIRECTIONAL_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT       8
#define MAX_SPOT_LIGHT_COUNT        8

const int kParallaxMappingDepthLayerCountMin = 8;
const int kParallaxMappingDepthLayerCountMax = 32;

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
  sampler2D normalMap;
  sampler2D depthMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} MATERIAL;

in vec3 fWorldPos;
in vec3 fNormal;
in mat3 fTBN;
in vec2 fTexCoords;
in vec3 fViewDirTangent;

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
                    vec3 color,
                    vec2 texCoords) {
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
                         vec3 color,
                         vec2 texCoords) {
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

void calcDirectionalLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index, vec2 texCoords) {
  vec3 L = normalize(-DIRECTIONAL_LIGHTS[index].dir);

  // Calculating light distance attenuation
  float attenuation = 1.0f;

  vec3 color = normalize(DIRECTIONAL_LIGHTS[index].color) * DIRECTIONAL_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color, texCoords);
}

void calcPointLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index, vec2 texCoords) {
  vec3 L = normalize(POINT_LIGHTS[index].worldPos - fWorldPos);

  // Calculating light distance attenuation
  float attenuation = calcLightAttenuation(
      POINT_LIGHTS[index].worldPos,
      POINT_LIGHTS[index].linAttCoef,
      POINT_LIGHTS[index].quadAttCoef
  );

  vec3 color = normalize(POINT_LIGHTS[index].color) * POINT_LIGHTS[index].intensity;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color, texCoords);
}

void calcSpotLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index, vec2 texCoords) {
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
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color, texCoords);
}
vec2 calcParallaxCoords() {
  // Calculating parallax mapping depth layer count
  int depthLayerCount = int(mix(
    kParallaxMappingDepthLayerCountMin,
    kParallaxMappingDepthLayerCountMax,
    max(fViewDirTangent.z, 0.0f)
  ));

  // Calculating P vector
  float depth = texture(MATERIAL.depthMap, fTexCoords).r * MATERIAL.parallaxStrength;
  vec2  P     = fViewDirTangent.xy * depth;
  //vec2  P     = fViewDirTangent.xy / fViewDirTangent.z * depth;

  // Calculating variables for steep parallax mapping
  vec2  layerTexStep       = P / depthLayerCount;
  float layerDepthStep     = 1.0f / depthLayerCount;
  vec2  currLayerTexCoords = fTexCoords;
  float currLayerDepth     = 0.0f;
  float currDepthMapValue  = texture(MATERIAL.depthMap, fTexCoords).r;

  // While current layer depth is less than current depth map value
  while (currLayerDepth < currDepthMapValue) {
    currLayerTexCoords -= layerTexStep;
    currLayerDepth     += layerDepthStep;
    currDepthMapValue   = texture(MATERIAL.depthMap, currLayerTexCoords).r;
  }

  // Parallax occlusion mapping
  vec2  prevLayerTexCoords  = currLayerTexCoords + layerTexStep;
  float prevLayerDepth      = currLayerDepth - layerDepthStep;
  float currDeltaDepth      = currDepthMapValue - currLayerDepth;
  float prevDeltaDepth      = prevLayerDepth - texture(MATERIAL.depthMap, prevLayerTexCoords).r;
  float interpolationWeight = prevDeltaDepth / (prevDeltaDepth + currDeltaDepth);
  vec2  texCoords           = mix(prevLayerTexCoords, currLayerTexCoords, interpolationWeight);

  return texCoords;
}

// Fragment shader
void main() {
  // Calculating texel coordinates using parallax mapping
  vec2 texCoords = calcParallaxCoords();

  // Discarding fragment if texel coordinates are out of bound
  //if (texCoords.x < 0.0f || texCoords.x > 1.0f || texCoords.y < 0.0f || texCoords.y > 1.0f) discard;

  // Initializing Phong/Blinn-Phong light model components
  vec3 ambient  = AMBIENT_LIGHT.color * AMBIENT_LIGHT.intensity
                * texture(MATERIAL.ambOccMap, texCoords).r
                * MATERIAL.ambCoef;
  vec3 diffuse  = vec3(0.0f);
  vec3 specular = vec3(0.0f);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(fTBN * (vec3(texture(MATERIAL.normalMap, texCoords)) * 2.0f - 1.0f));

  // Adding each directional light contribution
  for (uint i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcDirectionalLight(deltaDiffuse, deltaSpecular, N, i, texCoords);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each point light contribution
  for (uint i = 0; i < MAX_POINT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcPointLight(deltaDiffuse, deltaSpecular, N, i, texCoords);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each spot light contribution
  for (uint i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcSpotLight(deltaDiffuse, deltaSpecular, N, i, texCoords);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Add together all light components
  vec4 light = vec4(ambient + diffuse + specular, 1.0f);

  // Getting albedo and emission maps texels
  vec4 albedoTexel   = texture(MATERIAL.albedoMap, texCoords);
  vec4 emissionTexel = texture(MATERIAL.emissMap, texCoords);

  // Calculating fragment color by albedo map, light and also emission map
  FragColor = albedoTexel * light + emissionTexel;
}
