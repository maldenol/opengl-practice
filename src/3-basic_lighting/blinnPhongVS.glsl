#version 460 core

uniform vec3 VIEW_POS;

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;

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

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec3 fWorldPos;
out vec3 fNormal;
out vec2 fTexCoords;

// Vertex shader
void main() {
  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(mat3(transpose(inverse(MODEL))) * aNormal);

  // Calculating vertex world position
  vec4 worldPos = MODEL * vec4(aPos, 1.0f);

  // Passing interpolators to rasterizer
  fWorldPos       = vec3(worldPos);
  fNormal         = N;
  fTexCoords      = aTexCoords;

  // Setting vertex position
  gl_Position = PROJ * VIEW * worldPos;
}
