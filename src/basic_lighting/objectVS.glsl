#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform struct {
  float ambCoef;
  float diffCoef;
  float specCoef;
  float glossiness;
  float maxHeight;

  sampler2D texture;
  sampler2D normalMap;
  sampler2D heightMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} material;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;

out vec3 fWorldPos;
out vec3 fNormal;
out vec3 fTangent;
out vec2 fTexCoords;

void main() {
  vec3 normal    = normalize(vec3(model * vec4(aNormal, 0.0f)));
  vec3 tangent   = normalize(vec3(model * vec4(aTangent, 0.0f)));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  vec3 N = normalize(TBN * (vec3(texture(material.normalMap, aTexCoords)) * 2.0f - 1.0f));
  //vec3 N = mat3(transpose(inverse(model))) * aNormal;
  vec3 height = material.maxHeight * N * (texture(material.heightMap, aTexCoords).r * 2.0f - 1.0f);

  fWorldPos   = vec3(model * vec4(aPos + height, 1.0f));
  fNormal     = aNormal;
  fTangent    = aTangent;
  fTexCoords  = aTexCoords;
  gl_Position = proj * view * model * vec4(aPos + height, 1.0f);
}
