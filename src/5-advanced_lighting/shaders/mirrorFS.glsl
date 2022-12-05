#version 460 core

uniform float EXPOSURE;

uniform vec3 VIEW_POS;

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
  vec3 color = texture(MATERIAL.envMap, R).rgb;

  // Applying correction to cancel post-processing exposure tone mapping
  color *= 1.0f / EXPOSURE;

  FragColor = vec4(color, 1.0f);
}
