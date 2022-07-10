#version 460 core

const float kernelOffset = 0.001f;
const vec2 kernelOffsets[] = {
  vec2(-kernelOffset, kernelOffset),
  vec2(0.0f, kernelOffset),
  vec2(kernelOffset, kernelOffset),
  vec2(-kernelOffset, 0.0f),
  vec2(0.0f, 0.0f),
  vec2(kernelOffset, 0.0f),
  vec2(-kernelOffset, -kernelOffset),
  vec2(0.0f, -kernelOffset),
  vec2(kernelOffset, -kernelOffset),
};
const float kernelSharpen[] = {
  -1.0f, -1.0f, -1.0f,
  -1.0f,  9.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
};
const float kernelBlur[] = {
   1.0f / 16.0f,  2.0f / 16.0f,  1.0f / 16.0f,
   2.0f / 16.0f,  4.0f / 16.0f,  2.0f / 16.0f,
   1.0f / 16.0f,  2.0f / 16.0f,  1.0f / 16.0f,
};
const float kernelEdgeDetection[] = {
   1.0f,  1.0f,  1.0f,
   1.0f, -8.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
};

in vec2 fTexCoords;

out vec4 FragColor;

uniform sampler2D texture0;

void main() {
  // vec4 color = texture(texture0, fTexCoords);

  // Negative
  // color.r = 1.0f - color.r;
  // color.g = 1.0f - color.g;
  // color.b = 1.0f - color.b;

  // Black and white
  // float avr = (color.r + color.g + color.b) / 3.0f;
  // float avr = 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
  // color.r = avr;
  // color.g = avr;
  // color.b = avr;

  // Applying postprocessing kernels
  vec4 color = vec4(0.0f);
  for (uint i = 0; i < 9; ++i) {
    color += texture(texture0, fTexCoords + kernelOffsets[i]) * kernelSharpen[i];
  }

  FragColor = color;
}
