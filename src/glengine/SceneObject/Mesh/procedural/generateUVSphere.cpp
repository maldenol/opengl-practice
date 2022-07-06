// Header file
#include "../Mesh.hpp"

// STD
#include <cmath>

// Generates UV sphere mesh based on radius, level-of-detail, shader program and textures
glengine::Mesh glengine::generateUVSphere(float radius, unsigned int lod, GLuint shaderProgram,
                                          const std::vector<Mesh::Material::Texture> &textures) {
  // Incrementing level-of-detail
  ++lod;

  // Level-of-detail (count of rectangles per longitude/latitude)
  const unsigned int columns = 2 * lod;  // longitude
  const unsigned int rows    = lod;      // latitude

  const unsigned int trapezeCount = columns * rows;        // count of discrete trapezes
  const unsigned int vertexCount  = 4 * trapezeCount;      // count of vertices
  const unsigned int indexCount   = 2 * trapezeCount * 3;  // 3 indexes for each triangle
  const float        trapezeXDim =
      2.0f * M_PI / (float)columns;                 // maximal discrete trapeze's side x dimension
  const float trapezeYDim = M_PI / (float)rows;     // discrete trapeze's side y dimension
  const float trapezeUDim = 1.0f / (float)columns;  // discrete trapeze's side u dimension
  const float trapezeVDim = 1.0f / (float)rows;     // discrete trapeze's side v dimension

  std::vector<glm::vec3> vertices{};
  std::vector<glm::vec3> normals{};
  std::vector<glm::vec3> tangents{};
  std::vector<glm::vec2> uvs{};
  std::vector<GLuint>    indices{};
  vertices.resize(vertexCount);
  normals.resize(vertexCount);
  tangents.resize(vertexCount);
  uvs.resize(vertexCount);
  indices.resize(indexCount);

  // For each column (longitude)
  for (unsigned int c = 0; c < columns; ++c) {
    // For each row (latitude)
    for (unsigned int r = 0; r < rows; ++r) {
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

      // Transforming from Spherical coordinates to Cartesian
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

      tangents[vertexOffset]     = calculateTangent(std::vector<glm::vec3>{lu, ld, rd},
                                                    std::vector<glm::vec2>{luUV, ldUV, rdUV});
      tangents[vertexOffset + 1] = calculateTangent(std::vector<glm::vec3>{ld, rd, ru},
                                                    std::vector<glm::vec2>{ldUV, rdUV, ruUV});
      tangents[vertexOffset + 2] = calculateTangent(std::vector<glm::vec3>{rd, ru, lu},
                                                    std::vector<glm::vec2>{rdUV, ruUV, luUV});
      tangents[vertexOffset + 3] = calculateTangent(std::vector<glm::vec3>{ru, lu, ld},
                                                    std::vector<glm::vec2>{ruUV, luUV, ldUV});

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

  // Generating vertex buffer based on vertices, normals, tangents and uvs
  std::vector<float> vertexBuffer{generateVertexBuffer(vertices, normals, tangents, uvs)};

  // Configuring VBO attributes
  std::vector<Mesh::VBOAttribute> vboAttributes{};
  constexpr int                   kOffset = 11;
  vboAttributes.push_back(Mesh::VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(0)});
  vboAttributes.push_back(Mesh::VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(3 * sizeof(float))});
  vboAttributes.push_back(Mesh::VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(6 * sizeof(float))});
  vboAttributes.push_back(Mesh::VBOAttribute{2, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(9 * sizeof(float))});

  // Creating and returning the mesh
  return Mesh{
      vboAttributes, vertexBuffer, indices, shaderProgram,
      Mesh::Material{kInitAmbCoef, kInitDiffCoef, kInitSpecCoef, kInitGlossiness, kInitMaxHeight,
                     textures}
  };
}
