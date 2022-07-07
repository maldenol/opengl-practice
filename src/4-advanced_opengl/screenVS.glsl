#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 fTexCoords;

void main() {
  fTexCoords = aTexCoords;

  gl_Position = vec4(aPos.xy, 0.0f, 1.0f);
}
