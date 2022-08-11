#version 460 core

const float kSilhouetteSmoothingCoef = 0.5f;

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

layout (triangles, equal_spacing, ccw) in;

// Actual vertex shader
void vertex(vec3 normal, mat3 TBN, vec2 texCoords, vec3 pos) {
  // Calculating vertex world position
  vec4 worldPos = vec4(pos, 1.0f);

  // Passing interpolators to rasterizer
  o.worldPos       = vec3(worldPos);
  o.normal         = normal;
  o.TBN            = TBN;
  o.texCoords      = texCoords;
  o.viewDirTangent = normalize(transpose(TBN) * (VIEW_POS - vec3(worldPos)));

  // Setting vertex position
  gl_Position = PROJ * VIEW * worldPos;
}

// Interpolates intermediate point by control points and barycentric coordinates
vec2 interpolateVec2(vec2 cp1, vec2 cp2, vec2 cp3) {
  return cp1 * gl_TessCoord.x + cp2 * gl_TessCoord.y + cp3 * gl_TessCoord.z;
}

// Interpolates intermediate point by control points and barycentric coordinates
vec3 interpolateVec3(vec3 cp1, vec3 cp2, vec3 cp3) {
  return cp1 * gl_TessCoord.x + cp2 * gl_TessCoord.y + cp3 * gl_TessCoord.z;
}

// Interpolates intermediate point by control points and barycentric coordinates
mat3 interpolateMat3(mat3 cp1, mat3 cp2, mat3 cp3) {
  return cp1 * gl_TessCoord.x + cp2 * gl_TessCoord.y + cp3 * gl_TessCoord.z;
}

// Projects given point onto plane using any point on the plane and its normal
vec3 projectPointOntoPlane(vec3 point, vec3 planePoint, vec3 planePointNormal) {
  return point - dot(point - planePoint, planePointNormal) * planePointNormal;
}

// Calculates point by given control points and their normals
// applying silhouette smoothing using Phong smoothing
vec3 phongSmooth(vec3 cp1, vec3 cp2, vec3 cp3, vec3 cpn1, vec3 cpn2, vec3 cpn3) {
  // Performing usual calculations to find point on triangle
  vec3 pointOnTriangle = interpolateVec3(cp1, cp2, cp3);

  // Projecting point onto each control point tangent plane
  vec3 smoothedCP1 = projectPointOntoPlane(pointOnTriangle, cp1, cpn1);
  vec3 smoothedCP2 = projectPointOntoPlane(pointOnTriangle, cp2, cpn2);
  vec3 smoothedCP3 = projectPointOntoPlane(pointOnTriangle, cp3, cpn3);

  // Using projected points as new control points to find Phong-smoothed point
  vec3 smoothedPoint = interpolateVec3(smoothedCP1, smoothedCP2, smoothedCP3);

  return mix(pointOnTriangle, smoothedPoint, kSilhouetteSmoothingCoef);
}

// Tessellation evaluation shader
void main() {
  // Setting vertex indices
  const uint kIndices[3] = {0, 1, 2};

  // Getting intermediate point attributes
  vec3 normal    = interpolateVec3(i[kIndices[0]].normal, i[kIndices[1]].normal, i[kIndices[2]].normal);
  mat3 TBN       = interpolateMat3(i[kIndices[0]].TBN, i[kIndices[1]].TBN, i[kIndices[2]].TBN);
  vec2 texCoords = interpolateVec2(i[kIndices[0]].texCoords, i[kIndices[1]].texCoords, i[kIndices[2]].texCoords);
  vec3 pos       = phongSmooth(
      gl_in[kIndices[0]].gl_Position.xyz,
      gl_in[kIndices[1]].gl_Position.xyz,
      gl_in[kIndices[2]].gl_Position.xyz,
      i[kIndices[0]].normal,
      i[kIndices[1]].normal,
      i[kIndices[2]].normal
  );

  // Executing actual vertex shader
  vertex(normal, TBN, texCoords, pos);
}
