#version 460 core

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;

layout (location = 0) in vec3 aPos;
layout (location = 3) in vec2 aTexCoords;

out Interpolators {
  vec2 texCoords;
} o;

// Vertex shader
void main() {
  // Passing interpolators to rasterizer
  o.texCoords = aTexCoords;

  // Setting vertex position
  gl_Position   = PROJ * VIEW * MODEL * vec4(aPos, 1.0f);
  gl_Position.z = gl_Position.w;
}
