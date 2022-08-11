#version 460 core

uniform mat4 MODEL;

uniform bool INSTANCED;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoords;
layout (location = 4) in mat4 aModel;

out Vertex {
  vec3 normal;
  mat3 TBN;
  vec2 texCoords;
} o;

// Vertex shader
void main() {
  // Getting model matrix
  mat4 model = float(!INSTANCED) * MODEL + float(INSTANCED) * aModel;

  // Getting TBN matrix to transform normals from normal map from tangent space to world one
  vec3 normal  = normalize(vec3(model * vec4(aNormal, 0.0f)));
  vec3 tangent = normalize(vec3(model * vec4(aTangent, 0.0f)));
  // Gram-Schmidt orthogonalization
  tangent        = normalize(tangent - normal * dot(normal, tangent));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN       = mat3(tangent, bitangent, normal);

  o.normal    = aNormal;
  o.TBN       = TBN;
  o.texCoords = aTexCoords;

  // Passing position in world space
  gl_Position = model * vec4(aPos, 1.0f);
}
