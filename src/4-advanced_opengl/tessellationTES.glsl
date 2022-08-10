#version 460 core

uniform vec3 VIEW_POS;

uniform mat4 VIEW;
uniform mat4 PROJ;

uniform struct {
  float ambCoef;
  float diffCoef;
  float specCoef;

  float glossiness;

  float parallaxStrength;

  sampler2D albedoMap;
  sampler2D normalMap;
  sampler2D depthMap;
  sampler2D ambOccMap;
  sampler2D roughMap;
  sampler2D emissMap;
} MATERIAL;

in Vertex {
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} i[gl_MaxPatchVertices];

out Interpolators {
  vec3 worldPos;
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
  vec3 viewDirTangent;
} o;

layout (quads, fractional_odd_spacing, cw) in;

// Actual vertex shader
void vertex(vec3 normal, mat3 TBN, vec2 texCoords, vec3 pos) {
  // Using normal map and TBN matrix to get world space normal
  vec3 N = normalize(TBN * (texture(MATERIAL.normalMap, texCoords).xyz * 2.0f - 1.0f));

  // Using depth map and normal to get height displacement
  vec3 height = N * ((1.0f - texture(MATERIAL.depthMap, texCoords).r) * 2.0f - 1.0f) * MATERIAL.parallaxStrength;

  // Calculating vertex world position
  vec4 worldPos = vec4(pos + height, 1.0f);

  // Passing interpolators to rasterizer
  o.worldPos       = vec3(worldPos);
  o.normal         = normal;
  o.TBN            = TBN;
  o.texCoords      = texCoords;
  o.viewDirTangent = normalize(transpose(TBN) * (VIEW_POS - vec3(worldPos)));

  // Setting vertex position
  gl_Position = PROJ * VIEW * worldPos;
}

// Function to interpolate intermediate point by its coordinates relative to control points
vec2 interpolateVec2(vec2 cp00, vec2 cp01, vec2 cp10, vec2 cp11) {
  // Bilinearly interpolating
  vec2 ip0x = (cp01 - cp00) * gl_TessCoord.x + cp00;
  vec2 ip1x = (cp11 - cp10) * gl_TessCoord.x + cp10;
  vec2 ip   = (ip1x - ip0x) * gl_TessCoord.y + ip0x;

  return ip;
}

// Function to interpolate intermediate point by its coordinates relative to control points
vec3 interpolateVec3(vec3 cp00, vec3 cp01, vec3 cp10, vec3 cp11) {
  // Bilinearly interpolating
  vec3 ip0x = (cp01 - cp00) * gl_TessCoord.x + cp00;
  vec3 ip1x = (cp11 - cp10) * gl_TessCoord.x + cp10;
  vec3 ip   = (ip1x - ip0x) * gl_TessCoord.y + ip0x;

  return ip;
}

// Function to interpolate intermediate point by its coordinates relative to control points
mat3 interpolateMat3(mat3 cp00, mat3 cp01, mat3 cp10, mat3 cp11) {
  // Bilinearly interpolating
  mat3 ip0x = (cp01 - cp00) * gl_TessCoord.x + cp00;
  mat3 ip1x = (cp11 - cp10) * gl_TessCoord.x + cp10;
  mat3 ip   = (ip1x - ip0x) * gl_TessCoord.y + ip0x;

  return ip;
}

// Tessellation evaluation shader
void main() {
  // Setting vertex indices
  const uint indices[4] = {0, 1, 2, 3};

  // Getting intermediate point attributes
  vec3 normal    = interpolateVec3(i[indices[0]].normal, i[indices[1]].normal, i[indices[2]].normal, i[indices[3]].normal);
  mat3 TBN       = interpolateMat3(i[indices[0]].TBN, i[indices[1]].TBN, i[indices[2]].TBN, i[indices[3]].TBN);
  vec2 texCoords = interpolateVec2(i[indices[0]].texCoords, i[indices[1]].texCoords, i[indices[2]].texCoords, i[indices[3]].texCoords);
  vec3 pos       = interpolateVec3(
      gl_in[indices[0]].gl_Position.xyz,
      gl_in[indices[1]].gl_Position.xyz,
      gl_in[indices[2]].gl_Position.xyz,
      gl_in[indices[3]].gl_Position.xyz
  );

  // Executing actual vertex shader
  vertex(normal, TBN, texCoords, pos);
}
