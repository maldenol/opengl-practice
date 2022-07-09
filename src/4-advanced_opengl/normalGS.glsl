#version 460 core

const float MAGNITUDE = 0.05f;

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

in Interpolators {
  vec3 worldPos;
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} i[];

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

void generateNormal(uint index) {
  // Generating normal strip line
  gl_Position = PROJ * VIEW * gl_in[index].gl_Position;
  EmitVertex();
  vec3 N = mat3(transpose(inverse(MODEL))) * i[index].normal;
  gl_Position = PROJ * VIEW * (gl_in[index].gl_Position + vec4(MAGNITUDE * N, 0.0f));
  EmitVertex();
  EndPrimitive();
}

void main() {
  for (uint i = 0; i < 3; ++i) {
    generateNormal(i);
  }
}
