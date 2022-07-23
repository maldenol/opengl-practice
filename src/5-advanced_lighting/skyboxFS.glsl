#version 460 core

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

in Interpolators {
  vec2 texCoords;
} i;

out vec4 FragColor;

// Fragment shader
void main() {
  // Calculating fragment color by albedo map, color and also emission map
  FragColor = texture(MATERIAL.albedoMap, i.texCoords);
}
