#version 460 core

out vec4 FragColor;

void main() {
  FragColor = vec4(gl_FragCoord.zzz * 10.0f, 0.5f);
}
