#version 460 core

uniform vec3 LIGHT_COLOR;

out vec4 FragColor;

void main() {
  FragColor = vec4(LIGHT_COLOR, 1.0f);
}
