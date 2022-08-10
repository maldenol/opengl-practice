#version 460 core

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

layout (vertices = 4) out;

// Tessellation control shader
void main() {
  o[gl_InvocationID].normal    = i[gl_InvocationID].normal;
  o[gl_InvocationID].TBN       = i[gl_InvocationID].TBN;
  o[gl_InvocationID].texCoords = i[gl_InvocationID].texCoords;

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  if (gl_InvocationID == 0) {
    gl_TessLevelOuter[0] = 16.0f;
    gl_TessLevelOuter[1] = 16.0f;
    gl_TessLevelOuter[2] = 16.0f;
    gl_TessLevelOuter[3] = 16.0f;

    gl_TessLevelInner[0] = 16.0f;
    gl_TessLevelInner[1] = 16.0f;
  }
}
