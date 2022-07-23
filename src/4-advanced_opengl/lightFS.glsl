#version 460 core

uniform vec3 lightColor;

out vec4 FragColor;

void main() {
  FragColor = vec4(vec3(lightColor), 1.0f);
}
