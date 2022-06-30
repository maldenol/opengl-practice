// Header file
#include "./mesh.hpp"

// Generates vertex buffer based on vertices, normals and uvs
std::vector<float> glservice::generateVertexBuffer(std::vector<glm::vec3> vertices,
                                                   std::vector<glm::vec3> normals,
                                                   std::vector<glm::vec2> uvs) {
  constexpr int kOffset = 11;

  const int vertexCount = vertices.size();

  std::vector<float> vertexBuffer{};
  vertexBuffer.resize(kOffset * vertexCount);

  for (int i = 0; i < vertexCount; ++i) {
    vertexBuffer[i * kOffset]      = vertices[i].x;
    vertexBuffer[i * kOffset + 1]  = vertices[i].y;
    vertexBuffer[i * kOffset + 2]  = vertices[i].z;
    vertexBuffer[i * kOffset + 3]  = normals[i].x;
    vertexBuffer[i * kOffset + 4]  = normals[i].y;
    vertexBuffer[i * kOffset + 5]  = normals[i].z;
    vertexBuffer[i * kOffset + 6]  = uvs[i].x;
    vertexBuffer[i * kOffset + 7]  = uvs[i].y;
    vertexBuffer[i * kOffset + 8]  = 1.0f;
    vertexBuffer[i * kOffset + 9]  = 1.0f;
    vertexBuffer[i * kOffset + 10] = 1.0f;
  }

  return vertexBuffer;
}
