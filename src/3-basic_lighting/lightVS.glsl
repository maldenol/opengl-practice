#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) in vec3 aPos;

void main() {
  gl_Position = proj * view * model * vec4(aPos.xyz, 1.0f);
}
