#version 460 core

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;

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
layout (location = 3) in vec2 aTexCoords;

out Interpolators {
  vec2 texCoords;
} o;

// Vertex shader
void main() {
  // Passing interpolators to rasterizer
  o.texCoords = aTexCoords;

  // Calculating vertex position in clip space by vertex position and MVP transformation
  gl_Position   = PROJ * VIEW * MODEL * vec4(aPos, 1.0f);
  gl_Position.z = gl_Position.w;
}
