// Header file
#include "./mesh.hpp"

// STD
#include <cmath>

// Generates UV sphere mesh based on radius, level-of-detail, textures and shader program
glservice::Mesh glservice::generateUVSphere(float radius, int lod,
                                            const std::vector<GLuint> &textures,
                                            GLuint                     shaderProgram) {
  // Incrementing level-of-detail
  ++lod;

  // Level-Of-Details (count of rectangles per longitude/latitude)
  const int columns = 2 * lod;  // longitude
  const int rows    = lod;      // latitude

  const int   trapezeCount = columns * rows;        // count of discrete trapezes
  const int   vertexCount  = 4 * trapezeCount;      // count of vertices
  const int   indexCount   = 2 * trapezeCount * 3;  // 3 indexes for each triangle
  const float trapezeXDim =
      2.0f * M_PI / (float)columns;                 // maximal discrete trapeze's side x dimension
  const float trapezeYDim = M_PI / (float)rows;     // discrete trapeze's side y dimension
  const float trapezeUDim = 1.0f / (float)columns;  // discrete trapeze's side u dimension
  const float trapezeVDim = 1.0f / (float)rows;     // discrete trapeze's side v dimension

  std::vector<glm::vec3> vertices{};
  std::vector<glm::vec3> normals{};
  std::vector<glm::vec2> uvs{};
  std::vector<GLuint>    indices{};
  vertices.resize(vertexCount);
  normals.resize(vertexCount);
  uvs.resize(vertexCount);
  indices.resize(indexCount);

  for (int c = 0; c < columns; ++c) {
    for (int r = 0; r < rows; ++r) {
      const float luX = static_cast<float>(c) * trapezeXDim;
      const float luY = static_cast<float>(r + 1) * trapezeYDim;
      const float ruX = static_cast<float>(c + 1) * trapezeXDim;
      const float ruY = static_cast<float>(r + 1) * trapezeYDim;
      const float ldX = static_cast<float>(c) * trapezeXDim;
      const float ldY = static_cast<float>(r) * trapezeYDim;
      const float rdX = static_cast<float>(c + 1) * trapezeXDim;
      const float rdY = static_cast<float>(r) * trapezeYDim;

      const float luU = 1.0f - static_cast<float>(c) * trapezeUDim;
      const float luV = 1.0f - static_cast<float>(r + 1) * trapezeVDim;
      const float ruU = 1.0f - static_cast<float>(c + 1) * trapezeUDim;
      const float ruV = 1.0f - static_cast<float>(r + 1) * trapezeVDim;
      const float ldU = 1.0f - static_cast<float>(c) * trapezeUDim;
      const float ldV = 1.0f - static_cast<float>(r) * trapezeVDim;
      const float rdU = 1.0f - static_cast<float>(c + 1) * trapezeUDim;
      const float rdV = 1.0f - static_cast<float>(r) * trapezeVDim;

      // Spherical coordinates to Cartesian (formulas from Wikipedia)
      glm::vec3 lu{-std::cos(luX) * std::sin(luY), std::cos(luY), std::sin(luX) * std::sin(luY)};
      glm::vec3 ru{-std::cos(ruX) * std::sin(ruY), std::cos(ruY), std::sin(ruX) * std::sin(ruY)};
      glm::vec3 ld{-std::cos(ldX) * std::sin(ldY), std::cos(ldY), std::sin(ldX) * std::sin(ldY)};
      glm::vec3 rd{-std::cos(rdX) * std::sin(rdY), std::cos(rdY), std::sin(rdX) * std::sin(rdY)};

      glm::vec2 luUV{luU, luV};
      glm::vec2 ruUV{ruU, ruV};
      glm::vec2 ldUV{ldU, ldV};
      glm::vec2 rdUV{rdU, rdV};

      const int index        = c * rows + r;
      const int vertexOffset = index * 4;
      const int indexOffset  = index * 6;

      vertices[vertexOffset]     = lu * radius;
      vertices[vertexOffset + 1] = ru * radius;
      vertices[vertexOffset + 2] = ld * radius;
      vertices[vertexOffset + 3] = rd * radius;

      normals[vertexOffset]     = lu;
      normals[vertexOffset + 1] = ru;
      normals[vertexOffset + 2] = ld;
      normals[vertexOffset + 3] = rd;

      uvs[vertexOffset]     = luUV;
      uvs[vertexOffset + 1] = ruUV;
      uvs[vertexOffset + 2] = ldUV;
      uvs[vertexOffset + 3] = rdUV;

      indices[indexOffset]     = vertexOffset;      // left-up triangle
      indices[indexOffset + 1] = vertexOffset + 1;  // left-up triangle
      indices[indexOffset + 2] = vertexOffset + 2;  // left-up triangle
      indices[indexOffset + 3] = vertexOffset + 1;  // right-down triangle
      indices[indexOffset + 4] = vertexOffset + 3;  // right-down triangle
      indices[indexOffset + 5] = vertexOffset + 2;  // right-down triangle
    }
  }

  // Generating vertex buffer based on vertices, normals and uvs
  std::vector<float> vertexBuffer = glservice::generateVertexBuffer(vertices, normals, uvs);

  // Generating the mesh
  std::vector<VBOAttribute> vboAttributes{};
  constexpr int             kOffset = 11;
  vboAttributes.push_back(
      VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float), reinterpret_cast<void *>(0)});
  vboAttributes.push_back(VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                       reinterpret_cast<void *>(3 * sizeof(float))});
  vboAttributes.push_back(VBOAttribute{2, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                       reinterpret_cast<void *>(6 * sizeof(float))});
  vboAttributes.push_back(VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                       reinterpret_cast<void *>(8 * sizeof(float))});

  return generateMesh(vboAttributes, vertexBuffer, indices, textures, shaderProgram);
}
