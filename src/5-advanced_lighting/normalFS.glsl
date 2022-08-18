#version 460 core

uniform float EXPOSURE;

out vec4 FragColor;

void main() {
  vec3 color = vec3(1.0f, 1.0f, 0.0f);

  color *= 1.0f / EXPOSURE;

  FragColor = vec4(color, 1.0f);
}
