#version 460 core

const float kTessLevel = 4.0f;

in Vertex {
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} i[];

out Vertex {
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} o[];

layout (vertices = 3) out;

// Tessellation control shader
void main() {
  o[gl_InvocationID].normal    = i[gl_InvocationID].normal;
  o[gl_InvocationID].TBN       = i[gl_InvocationID].TBN;
  o[gl_InvocationID].texCoords = i[gl_InvocationID].texCoords;

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  // Configuring tessellation levels
  if (gl_InvocationID == 0) {
    gl_TessLevelOuter[0] = kTessLevel;
    gl_TessLevelOuter[1] = kTessLevel;
    gl_TessLevelOuter[2] = kTessLevel;

    gl_TessLevelInner[0] = kTessLevel;
  }
}
