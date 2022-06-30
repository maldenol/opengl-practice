#version 460 core

uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;

out vec3 fColor;
out vec2 fTexCoords;

void main() {
  fColor = aColor * lightColor;
  fTexCoords = aTexCoords;
  gl_Position = proj * view * model * vec4(aPos.xyz, 1.0f);
}
