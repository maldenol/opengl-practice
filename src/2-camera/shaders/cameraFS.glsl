#version 460 core

uniform sampler2D texture0;

in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragColor;

void main() {
  FragColor = texture(texture0, fTexCoords) * vec4(fColor, 1.0f);
}
