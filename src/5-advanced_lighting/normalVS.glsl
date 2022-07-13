#version 460 core

uniform mat4 MODEL;

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

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

out Interpolators {
  vec3 worldPos;
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} o;

// Vertex shader
void main() {
  // Getting TBN matrix to transform normals from normal map from tangent space to world one
  vec3 normal    = normalize(vec3(MODEL * vec4(aNormal, 0.0f)));
  vec3 tangent   = normalize(vec3(MODEL * vec4(aTangent, 0.0f)));
  // Gram-Schmidt orthogonalization
  tangent        = normalize(tangent - normal * dot(normal, tangent));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(TBN * (vec3(texture(MATERIAL.normalMap, aTexCoords)) * 2.0f - 1.0f));
  //vec3 N = normalize(mat3(transpose(inverse(MODEL))) * aNormal);
  // Using height map and normal to get world space height vector
  vec3 height = MATERIAL.maxHeight * N * (texture(MATERIAL.heightMap, aTexCoords).r * 2.0f - 1.0f);

  // Calculating vertex world position
  vec4 worldPos = MODEL * vec4(aPos, 1.0f) + vec4(height, 0.0f);

  // Passing interpolators to rasterizer
  o.worldPos   = vec3(worldPos);
  o.normal     = aNormal;
  o.TBN        = TBN;
  o.texCoords  = aTexCoords;

  // Calculating vertex position in clip space by vertex position,
  // MVP transformation and height vector
  gl_Position = worldPos;
}
