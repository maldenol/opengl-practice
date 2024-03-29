#version 460 core

const float kKernelOffset = 0.001f;
const vec2 kKernelOffsets[9] = {
  vec2(-kKernelOffset, kKernelOffset),
  vec2(0.0f, kKernelOffset),
  vec2(kKernelOffset, kKernelOffset),
  vec2(-kKernelOffset, 0.0f),
  vec2(0.0f, 0.0f),
  vec2(kKernelOffset, 0.0f),
  vec2(-kKernelOffset, -kKernelOffset),
  vec2(0.0f, -kKernelOffset),
  vec2(kKernelOffset, -kKernelOffset),
};
const float kKernelSharpen[] = {
  -1.0f, -1.0f, -1.0f,
  -1.0f,  9.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
};
const float kKernelBlur[] = {
   1.0f / 16.0f,  2.0f / 16.0f,  1.0f / 16.0f,
   2.0f / 16.0f,  4.0f / 16.0f,  2.0f / 16.0f,
   1.0f / 16.0f,  2.0f / 16.0f,  1.0f / 16.0f,
};
const float kKernelEdgeDetection[] = {
   1.0f,  1.0f,  1.0f,
   1.0f, -8.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
};

//uniform float BIGGEST_RADIANCE;
uniform float EXPOSURE;

uniform sampler2D texture0;

in vec2 fTexCoords;

out vec4 FragColor;

void main() {
  // Getting post-processing texture texel
  vec4 color = texture(texture0, fTexCoords);

  // Negative
  //color.r = 1.0f - color.r;
  //color.g = 1.0f - color.g;
  //color.b = 1.0f - color.b;

  // Black and white
  //float avr = (color.r + color.g + color.b) / 3.0f;
  //float avr = 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
  //color.r   = avr;
  //color.g   = avr;
  //color.b   = avr;

  // Applying postprocessing kernels
  //vec4 color = vec4(0.0f);
  //for (uint i = 0; i < 9; ++i) {
  //  color += texture(texture0, fTexCoords + kKernelOffsets[i]) * kKernelSharpen[i];
  //}

  // Applying tone mapping (Reinhard algorithm)
  //float luminanceIn  = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
  ////float luminanceOut = luminanceIn / (1.0f + luminanceIn); // shortened
  //float luminanceOut = luminanceIn / (1.0f + luminanceIn)
  //                   * (1.0f + luminanceIn / (BIGGEST_RADIANCE * BIGGEST_RADIANCE));
  //color.rgb          = color.rgb * luminanceOut / luminanceIn;

  // Applying tone mapping (exposure)
  color.rgb = vec3(1.0f) - exp(-color.rgb * EXPOSURE);

  // Applying gamma correction
  float gammaR = 2.2f;
  float gammaG = 2.2f;
  float gammaB = 2.2f;
  color.rgb    = pow(color.rgb, vec3(1.0f / gammaR, 1.0f / gammaG, 1.0f / gammaB));

  FragColor = color;
}
