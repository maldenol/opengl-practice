#version 460 core

const float kMagnitude = 0.05f;

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
  vec3 N      = normalize(mat3(transpose(inverse(MODEL))) * i[index].normal);
  gl_Position = PROJ * VIEW * (gl_in[index].gl_Position + vec4(kMagnitude * N, 0.0f));
  EmitVertex();
  EndPrimitive();
}

// Geometry shader
void main() {
  for (uint i = 0; i < 3; ++i) {
    generateNormal(i);
  }
}
