#version 460 core

uniform vec3  lightColor;
uniform float lightIntensity;

out vec4 FragColor;

void main() {
  FragColor = vec4(vec3(lightColor * lightIntensity), 1.0f);
}
