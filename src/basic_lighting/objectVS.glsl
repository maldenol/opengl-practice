#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform struct {
  float ambCoef;
  float diffCoef;
  float specCoef;
  float glossiness;

  sampler2D texture;
  sampler2D normalMap;
  sampler2D heightMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} material;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

out vec3 fWorldPos;
out vec3 fNormal;
out vec3 fColor;
out vec2 fTexCoords;

void main() {
  fWorldPos   = aPos;
  fNormal     = aNormal;
  fColor      = aColor;
  fTexCoords  = aTexCoords;
  gl_Position = proj * view * model * vec4(aPos.xyz, 1.0f);
}
