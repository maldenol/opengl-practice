// Header file
#include "./mesh.hpp"

// STD
#include <cmath>

// Generates icosphere mesh based on radius, textures and shader program
glservice::Mesh glservice::generateIcoSphere(float radius, const std::vector<GLuint> &textures,
                                             GLuint shaderProgram) {
  constexpr int kVertexCount = 12;      // count of vertices
  constexpr int kIndexCount  = 20 * 3;  // 3 indexes for each triangle

  std::vector<glm::vec3> vertices{};
  std::vector<glm::vec3> normals{};
  std::vector<glm::vec2> uvs{};
  std::vector<GLuint>    indices{};
  vertices.resize(kVertexCount);
  normals.resize(kVertexCount);
  uvs.resize(kVertexCount);
  indices.resize(kIndexCount);

  // Generating an icosahedron
  float circumferenceRadius = 1.0f / std::sqrt(5.0f);
  for (int p = 0; p < 2; ++p) {  // 2 poles
    constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};

    for (int v = 0; v < 5; ++v) {  // single pole vertices
      const float lerpByIndex     = v / 5.0f;
      const float angle           = 2.0f * M_PI * lerpByIndex;
      const int   currVertexIndex = p * 6 + v;

      vertices[currVertexIndex] =
          glm::vec3{circumferenceRadius * std::cos(angle), -0.5f * (1.0f - circumferenceRadius),
                    circumferenceRadius * std::sin(angle)};

      if (p == 0) {  // upper
        uvs[currVertexIndex] = glm::vec2{lerpByIndex, 2.0f / 3.0f};
      } else {  // lower
        uvs[currVertexIndex] = glm::vec2{1.0f - lerpByIndex, 1.0f / 3.0f};
      }
    }

    const int poleIndex = p * 6 + 5;

    // Pole vertex
    vertices[poleIndex] = glm::vec3{0.0f, 0.0f, 0.0f};

    if (p == 0) {  // upper
      uvs[poleIndex] = glm::vec2{0.0f, 1.0f};
    } else {  // lower
      uvs[poleIndex] = glm::vec2{0.0f, 0.0f};
    }

    for (int t = 0; t < 5; ++t) {  // pole indices
      int currVertexIndex = p * 6 + t;
      int nextVertexIndex = p * 6 + (t + 1) % 5;
      int indexOffset     = (p * 5 + t) * 3;

      indices[indexOffset]     = currVertexIndex;
      indices[indexOffset + 1] = poleIndex;
      indices[indexOffset + 2] = nextVertexIndex;
    }

    if (p == 0) {  // upper pole transformation
      for (int v = 0; v < 6; ++v) {
        vertices[v].y += 0.5f;
        vertices[v] = glm::angleAxis(glm::radians(-144.0f), kUp) * vertices[v];
      }
    } else {  // lower pole transformation
      for (int v = 0; v < 6; ++v) {
        vertices[6 + v].y *= -1.0f;
        vertices[6 + v].y -= 0.5f;
        vertices[6 + v].x *= -1.0f;
      }
    }
  }

  for (int tp = 0; tp < 5; ++tp) {  // side indices (in pairs)
    const int currUpperVertexIndex = tp;
    const int nextUpperVertexIndex = (tp + 1) % 5;
    const int currLowerVertexIndex = 6 + (5 - tp) % 5;
    const int nextLowerVertexIndex = 6 + (10 - (tp + 1)) % 5;
    const int indexOffset          = 30 + tp * 6;

    indices[indexOffset]     = currUpperVertexIndex;
    indices[indexOffset + 1] = nextUpperVertexIndex;
    indices[indexOffset + 2] = currLowerVertexIndex;
    indices[indexOffset + 3] = nextUpperVertexIndex;
    indices[indexOffset + 4] = nextLowerVertexIndex;
    indices[indexOffset + 5] = currLowerVertexIndex;
  }

  // Projecting icosahedron on sphere
  for (int v = 0; v < kVertexCount; ++v) {
    normals[v]  = glm::normalize(vertices[v]);
    vertices[v] = normals[v] * radius;
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
