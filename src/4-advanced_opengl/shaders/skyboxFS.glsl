#version 460 core

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
  vec2 texCoords;
} i;

out vec4 FragColor;

// Fragment shader
void main() {
  // Calculating fragment color by albedo map, color and also emission map
  FragColor = texture(MATERIAL.albedoMap, i.texCoords);
}
