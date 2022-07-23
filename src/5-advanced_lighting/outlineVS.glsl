#version 460 core

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;

layout (location = 0) in vec3 aPos;

void main() {
  gl_Position = PROJ * VIEW * MODEL * vec4(aPos.xyz * 1.1f, 1.0f);
}
