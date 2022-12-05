#version 460 core

uniform vec3 VIEW_POS;

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;

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

uniform bool INSTANCED;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in mat4 aModel;

out Interpolators {
  vec3 worldPos;
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
  vec3 viewDirTangent;
} o;

// Vertex shader
void main() {
  // Getting model matrix
  mat4 model = float(!INSTANCED) * MODEL + float(INSTANCED) * aModel;

  // Getting TBN matrix to transform normals from normal map from tangent space to world one
  vec3 normal  = normalize(vec3(model * vec4(aNormal, 0.0f)));
  vec3 tangent = normalize(vec3(model * vec4(aTangent, 0.0f)));
  // Gram-Schmidt orthogonalization
  tangent        = normalize(tangent - normal * dot(normal, tangent));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(TBN * (texture(MATERIAL.normalMap, aTexCoords).xyz * 2.0f - 1.0f));
  //vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);

  // Calculating vertex world position
  vec4 worldPos = model * vec4(aPos, 1.0f);

  // Passing interpolators to rasterizer
  o.worldPos       = vec3(worldPos);
  o.normal         = aNormal;
  o.TBN            = TBN;
  o.texCoords      = aTexCoords;
  o.viewDirTangent = normalize(transpose(TBN) * (VIEW_POS - vec3(worldPos)));

  // Setting vertex position
  gl_Position = PROJ * VIEW * worldPos;
}
