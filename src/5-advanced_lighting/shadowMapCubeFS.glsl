#version 460 core

uniform struct {
  vec3  worldPos;
  float farPlane;
} LIGHT;

in vec3 vertexWorldPos;

// Fragment shader
void main() {
  gl_FragDepth = length(LIGHT.worldPos - vertexWorldPos) / LIGHT.farPlane;
}
