#version 460 core

uniform float EXPOSURE;

out vec4 FragColor;

void main() {
  vec3 color = gl_FragCoord.zzz;

  color *= 1.0f / EXPOSURE;

  FragColor = vec4(color, 0.5f);
}
