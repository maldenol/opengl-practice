#version 460 core

#define MAX_DIRECTIONAL_LIGHT_COUNT 8
#define MAX_POINT_LIGHT_COUNT       8
#define MAX_SPOT_LIGHT_COUNT        8

const int kParallaxMappingDepthLayerCountMin = 8;
const int kParallaxMappingDepthLayerCountMax = 32;

uniform vec3 VIEW_POS;

uniform vec3 AMBIENT_LIGHT_COLOR;
uniform struct {
  vec3 color;

  vec3 dir;
} DIRECTIONAL_LIGHTS[MAX_DIRECTIONAL_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3 color;

  float linAttCoef;
  float quadAttCoef;
} POINT_LIGHTS[MAX_POINT_LIGHT_COUNT];
uniform struct {
  vec3 worldPos;

  vec3 color;

  vec3 dir;

  float linAttCoef;
  float quadAttCoef;

  float angle;
  float smoothAngle;
} SPOT_LIGHTS[MAX_SPOT_LIGHT_COUNT];

uniform struct {
  sampler2D   albedoMap;
  sampler2D   normalMap;
  sampler2D   depthMap;
  sampler2D   ambOccMap;
  sampler2D   glossMap;
  sampler2D   emissMap;
  samplerCube envMap;

  float parallaxStrength;
} MATERIAL;

in Interpolators {
  vec3 worldPos;
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
  vec3 viewDirTangent;
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
          * attenuation;
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
  vec3 V = normalize(VIEW_POS - i.worldPos);

  float gloss    = texture(MATERIAL.glossMap, texCoords).r;
  float glossExp = exp2(gloss);
  float VdotR    = max(dot(V, R), 0.0f);

  // Calculating specular (Phong) light
  specular = color
           * gloss * pow(VdotR, glossExp)
           * attenuation;
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

  float gloss    = texture(MATERIAL.glossMap, texCoords).r;
  float glossExp = 2.0f * exp2(gloss);
  vec3  V        = normalize(VIEW_POS - i.worldPos);
  vec3  H        = normalize(L + V);
  float HdotN    = max(dot(H, N), 0.0f);
  float LdotN    = max(dot(L, N), 0.0f);

  // Calculating specular (Blinn-Phong) light
  specular = color
           * gloss * pow(HdotN, glossExp) * float(LdotN > 0.0f)
           * attenuation;
}

float calcLightAttenuation(vec3 worldPos, float linAttCoef, float quadAttCoef) {
  float dist = length(worldPos - i.worldPos);

  return 1.0f / (1.0f + linAttCoef * dist + quadAttCoef * dist * dist);
}

void calcDirectionalLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index, vec2 texCoords) {
  vec3 L = normalize(-DIRECTIONAL_LIGHTS[index].dir);

  float attenuation = 1.0f;

  vec3 color = DIRECTIONAL_LIGHTS[index].color;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color, texCoords);
}

void calcPointLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index, vec2 texCoords) {
  vec3 L = normalize(POINT_LIGHTS[index].worldPos - i.worldPos);

  // Calculating light distance attenuation
  float attenuation = calcLightAttenuation(
      POINT_LIGHTS[index].worldPos,
      POINT_LIGHTS[index].linAttCoef,
      POINT_LIGHTS[index].quadAttCoef
  );

  vec3 color = POINT_LIGHTS[index].color;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color, texCoords);
}

void calcSpotLight(out vec3 diffuse, out vec3 specular, vec3 N, uint index, vec2 texCoords) {
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

  vec3 color = SPOT_LIGHTS[index].color;

  // Calculation diffuse and specular light (Blinn-Phong)
  calcBlinnPhongLight(diffuse, specular, N, L, attenuation, color, texCoords);
}

vec2 calcParallaxCoords() {
  // Calculating parallax mapping depth layer count
  int depthLayerCount = int(mix(
    kParallaxMappingDepthLayerCountMin,
    kParallaxMappingDepthLayerCountMax,
    max(i.viewDirTangent.z, 0.0f)
  ));

  // Calculating P vector
  float depth = texture(MATERIAL.depthMap, i.texCoords).r * MATERIAL.parallaxStrength;
  vec2  P     = i.viewDirTangent.xy * depth;
  //vec2  P     = i.viewDirTangent.xy / i.viewDirTangent.z * depth;

  // Calculating variables for steep parallax mapping
  vec2  layerTexStep       = P / depthLayerCount;
  float layerDepthStep     = 1.0f / depthLayerCount;
  vec2  currLayerTexCoords = i.texCoords;
  float currLayerDepth     = 0.0f;
  float currDepthMapValue  = texture(MATERIAL.depthMap, i.texCoords).r;

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

vec4 triplanarMap(sampler2D map, vec3 worldPos, vec3 normal) {
  vec3 triplanarWeights = abs(normal);
  triplanarWeights      = triplanarWeights
                        / (triplanarWeights.x + triplanarWeights.y + triplanarWeights.z);
  return   texture(map, worldPos.yz) * triplanarWeights.x
         + texture(map, worldPos.xz) * triplanarWeights.y
         + texture(map, worldPos.xy) * triplanarWeights.z;
}

// Fragment shader
void main() {
  // Calculating texel coordinates using parallax mapping
  vec2 texCoords = calcParallaxCoords();

  // Discarding fragment if texel coordinates are out of bound
  if (texCoords.x < 0.0f || texCoords.x > 1.0f || texCoords.y < 0.0f || texCoords.y > 1.0f) discard;

  // Initializing Phong/Blinn-Phong light model components
  vec3 ambient  = AMBIENT_LIGHT_COLOR
                * texture(MATERIAL.ambOccMap, texCoords).r;
  vec3 diffuse  = vec3(0.0f);
  vec3 specular = vec3(0.0f);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(i.TBN * (vec3(texture(MATERIAL.normalMap, texCoords)) * 2.0f - 1.0f));

  // Adding each directional light contribution
  for (uint i = 0; i < MAX_DIRECTIONAL_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse  = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcDirectionalLight(deltaDiffuse, deltaSpecular, N, i, texCoords);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each point light contribution
  for (uint i = 0; i < MAX_POINT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse  = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcPointLight(deltaDiffuse, deltaSpecular, N, i, texCoords);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding each spot light contribution
  for (uint i = 0; i < MAX_SPOT_LIGHT_COUNT; ++i) {
    vec3 deltaDiffuse  = vec3(0.0f);
    vec3 deltaSpecular = vec3(0.0f);

    calcSpotLight(deltaDiffuse, deltaSpecular, N, i, texCoords);

    diffuse  += max(deltaDiffuse, vec3(0.0f));
    specular += max(deltaSpecular, vec3(0.0f));
  }

  // Adding together all the light components
  vec3 light = ambient + diffuse + specular;

  // Getting albedo and emission maps texels (UV-mapping)
  vec4 albedoTexel   = texture(MATERIAL.albedoMap, texCoords);
  vec4 emissionTexel = texture(MATERIAL.emissMap, texCoords);
  // Getting albedo and emission maps texels (triplanar mapping)
  //vec4 albedoTexel   = triplanarMap(MATERIAL.albedoMap, i.worldPos, N);
  //veс4 emissionTexel = triplanarMap(MATERIAL.emissMap, i.worldPos, N);

  // Calculating fragment color by albedo map, light and also emission map
  vec3 color = albedoTexel.rgb * light + emissionTexel.rgb * emissionTexel.a;

  FragColor = vec4(color, 1.0f);
}
