#version 460 core

uniform vec3 VIEW_POS;

uniform samplerCube SKYBOX;

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

// Fragment shader
void main() {
  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(i.TBN * (vec3(texture(MATERIAL.normalMap, i.texCoords)) * 2.0f - 1.0f));

  // Reflecting view direction
  vec3 V = normalize(VIEW_POS - i.worldPos);
  vec3 R = reflect(-V, N);
  // Fixing negative Y-axis
  R.y *= -1.0f;

  // Getting texel from cubemap at reflected vector
  FragColor = vec4(texture(SKYBOX, R).rgb, 1.0f);
}
