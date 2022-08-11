#version 460 core

const float kTessLevelMin = 1.0f;
const float kTessLevelMax = 16.0f;

const float KDistanceMin = 1.0f;
const float KDistanceMax = 20.0f;

uniform mat4 VIEW;

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

// Calculates tessellation level of edge by given two positions in view space
float calculateEdgeTessLevel(vec4 p1, vec4 p2) {
  float closerDistance = -min(p1.z, p2.z);
  float closerDistanceClamped = clamp(
    (closerDistance - KDistanceMin) / (KDistanceMax - KDistanceMin),
    0.0f,
    1.0f
  );

  return mix(kTessLevelMax, kTessLevelMin, closerDistanceClamped);
}

// Tessellation control shader
void main() {
  o[gl_InvocationID].normal    = i[gl_InvocationID].normal;
  o[gl_InvocationID].TBN       = i[gl_InvocationID].TBN;
  o[gl_InvocationID].texCoords = i[gl_InvocationID].texCoords;

  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  // Configuring tessellation levels
  if (gl_InvocationID == 0) {
    // Calculating vertex positions in view space
    vec4 viewPos0 = VIEW * gl_in[0].gl_Position;
    vec4 viewPos1 = VIEW * gl_in[1].gl_Position;
    vec4 viewPos2 = VIEW * gl_in[2].gl_Position;
    vec4 viewPos3 = VIEW * gl_in[3].gl_Position;

    // Calculating outer tessellation levels for each edge based on closer vertex distance
    float tessLevelOuter0 = calculateEdgeTessLevel(viewPos2, viewPos0);
    float tessLevelOuter1 = calculateEdgeTessLevel(viewPos0, viewPos1);
    float tessLevelOuter2 = calculateEdgeTessLevel(viewPos1, viewPos3);
    float tessLevelOuter3 = calculateEdgeTessLevel(viewPos3, viewPos2);

    // Calculating inner tessellation levels as maximum of tessellation levels of according edges
    float tessLevelInner0 = max(tessLevelOuter1, tessLevelOuter3);
    float tessLevelInner1 = max(tessLevelOuter0, tessLevelOuter2);

    // Setting tessellation levels
    gl_TessLevelOuter[0] = tessLevelOuter0;
    gl_TessLevelOuter[1] = tessLevelOuter1;
    gl_TessLevelOuter[2] = tessLevelOuter2;
    gl_TessLevelOuter[3] = tessLevelOuter3;

    gl_TessLevelInner[0] = tessLevelInner0;
    gl_TessLevelInner[1] = tessLevelInner1;
  }
}
