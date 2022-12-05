#version 460 core

const float kPosCoef  = 2.0f;
const float kTimeCoef = 0.1f;

uniform float time;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout (rgba8, binding = 0) writeonly uniform image2D outputTexture;

// 3D Simplex noise
float simplexNoise(vec3 pos);

// Compute shader
void main() {
  vec3 noisePos  = gl_GlobalInvocationID;
  noisePos.z    += time * kTimeCoef;
  noisePos.xyz  *= 1.0f / (gl_WorkGroupSize * gl_NumWorkGroups) * kPosCoef;

  vec3 rOffset = vec3(0.0f, 0.0f, 0.0f);
  vec3 gOffset = vec3(gl_WorkGroupSize.x * gl_NumWorkGroups.x, 0.0f, 0.0f);
  vec3 bOffset = vec3(0.0f, gl_WorkGroupSize.y * gl_NumWorkGroups.y, 0.0f);

  float r = simplexNoise(noisePos + rOffset);
  float g = simplexNoise(noisePos + gOffset);
  float b = simplexNoise(noisePos + bOffset);
  float a = 0.05f;

  imageStore(outputTexture, ivec2(gl_GlobalInvocationID.xy), vec4(r, g, b, a));
}

// 3D Simplex noise
vec4 permutate(vec4 x) { return mod(((x * 34.0f) + 1.0f) * x, 289.0f); }
vec4 taylorInverseSqrt(vec4 num) { return 1.79284291400159 - 0.85373472095314 * num; }
float simplexNoise(vec3 pos) {
  const vec2 C = vec2(1.0f / 6.0f, 1.0f / 3.0f);
  const vec4 D = vec4(0.0f, 0.5f, 1.0f, 2.0f);

  // First corner
  vec3 i  = floor(pos + dot(pos, C.yyy));
  vec3 x0 = pos - i + dot(i, C.xxx);

  // Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0f - g;
  vec3 i1 = min(g.xyz, l.zxy);
  vec3 i2 = max(g.xyz, l.zxy);

  // x0 = x0 - 0 + 0 * C
  vec3 x1 = x0 - i1 + 1.0f * C.xxx;
  vec3 x2 = x0 - i2 + 2.0f * C.xxx;
  vec3 x3 = x0 - 1.0f + 3.0f * C.xxx;

  // Performing permutations
  i = mod(i, 289.0f);
  vec4 p = permutate(
    permutate(
      permutate(
        i.z + vec4(0.0f, i1.z, i2.z, 1.0f)
      ) + i.y + vec4(0.0f, i1.y, i2.y, 1.0f)
    ) + i.x + vec4(0.0f, i1.x, i2.x, 1.0f)
  );

  // Calculating gradients
  // (N * N points uniformly over a square, mapped onto an octahedron)
  float n_ = 1.0f / 7.0f; // N = 7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0f * floor(p * ns.z * ns.z); // mod(p, N * N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0f * x_); // mod(j, N)

  vec4 x = x_ * ns.x + ns.yyyy;
  vec4 y = y_ * ns.x + ns.yyyy;
  vec4 h = 1.0f - abs(x) - abs(y);

  vec4 b0 = vec4(x.xy, y.xy);
  vec4 b1 = vec4(x.zw, y.zw);

  vec4 s0 = floor(b0) * 2.0f + 1.0f;
  vec4 s1 = floor(b1) * 2.0f + 1.0f;
  vec4 sh = -step(h, vec4(0.0f));

  vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
  vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

  vec3 p0 = vec3(a0.xy, h.x);
  vec3 p1 = vec3(a0.zw, h.y);
  vec3 p2 = vec3(a1.xy, h.z);
  vec3 p3 = vec3(a1.zw, h.w);

  // Normalising gradients
  vec4 norm = taylorInverseSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

  // Mix final noise value
  vec4 m = max(0.6f - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0f);
  m      = m * m * m * m;
  return 42.0f * dot(m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}
