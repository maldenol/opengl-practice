#version 460 core

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float     mixRatio = 0.0f;

in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragColor;

void main() {
  FragColor = mix(texture(texture0, fTexCoords), texture(texture1, fTexCoords), mixRatio)
   * vec4(fColor, 1.0f);
}
