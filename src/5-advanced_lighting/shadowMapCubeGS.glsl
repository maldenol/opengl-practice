#version 460 core

uniform mat4 LIGHT_VP[6];

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec3 vertexWorldPos;

// Geometry shader
void main() {
  // For each cube map face
  for (uint i = 0; i < 6; ++i) {
    // Specifying cube map face
    gl_Layer = int(i);

    // For each vertex of the triangle
    for (uint j = 0; j < 3; ++j) {
      vertexWorldPos = vec3(gl_in[j].gl_Position);
      gl_Position    = LIGHT_VP[i] * gl_in[j].gl_Position;
      EmitVertex();
    }
    EndPrimitive();
  }
}
