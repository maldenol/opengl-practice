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
  sampler2D normalMap;
  sampler2D depthMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} MATERIAL;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

out vec3 fWorldPos;
out vec3 fNormal;
out mat3 fTBN;
out vec2 fTexCoords;
out vec3 fViewDirTangent;

// Vertex shader
void main() {
  // Getting TBN matrix to transform normals from normal map from tangent space to world one
  vec3 normal  = normalize(vec3(MODEL * vec4(aNormal, 0.0f)));
  vec3 tangent = normalize(vec3(MODEL * vec4(aTangent, 0.0f)));
  // Gram-Schmidt orthogonalization
  tangent        = normalize(tangent - normal * dot(normal, tangent));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(TBN * (vec3(texture(MATERIAL.normalMap, aTexCoords)) * 2.0f - 1.0f));
  //vec3 N = normalize(mat3(transpose(inverse(MODEL))) * aNormal);

  // Calculating vertex world position
  vec4 worldPos = MODEL * vec4(aPos, 1.0f);

  // Passing interpolators to rasterizer
  fWorldPos       = vec3(worldPos);
  fNormal         = aNormal;
  fTBN            = TBN;
  fTexCoords      = aTexCoords;
  fViewDirTangent = normalize(transpose(TBN) * (VIEW_POS - vec3(worldPos)));

  // Setting vertex position
  gl_Position = PROJ * VIEW * worldPos;
}
