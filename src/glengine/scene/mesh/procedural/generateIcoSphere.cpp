// Header file
#include "../mesh.hpp"

// STD
#include <cmath>

// GLM
#include <glm/gtx/quaternion.hpp>

// Generates icosphere mesh based on radius, shader program and textures
glengine::Mesh glengine::generateIcoSphere(float radius, GLuint shaderProgram,
                                           const std::vector<Texture> &textures) {
  constexpr int kVertexCount = 12;      // count of vertices
  constexpr int kIndexCount  = 20 * 3;  // 3 indexes for each triangle

  std::vector<glm::vec3> vertices{};
  std::vector<glm::vec3> normals{};
  std::vector<glm::vec3> tangents{};
  std::vector<glm::vec2> uvs{};
  std::vector<GLuint>    indices{};
  vertices.resize(kVertexCount);
  normals.resize(kVertexCount);
  tangents.resize(kVertexCount);
  uvs.resize(kVertexCount);
  indices.resize(kIndexCount);

  // Generating the icosahedron
  float circumferenceRadius = 1.0f / std::sqrt(5.0f);
  // For each pole
  for (int p = 0; p < 2; ++p) {
    constexpr glm::vec3 kUp{0.0f, 1.0f, 0.0f};

    const int currPoleOffset = p * 6;

    // For each vertex of the hemisphere of appropriate pole except the pole vertex
    for (int v = 0; v < 5; ++v) {
      const float lerpByIndex     = v / 5.0f;
      const float angle           = 2.0f * M_PI * lerpByIndex;
      const int   currVertexIndex = currPoleOffset + v;

      // Creating a vertex
      vertices[currVertexIndex] =
          glm::vec3{circumferenceRadius * std::cos(angle), -0.5f * (1.0f - circumferenceRadius),
                    circumferenceRadius * std::sin(angle)};

      // Setting UVs of the vertex
      // If pole is the upper one
      if (p == 0) {
        uvs[currVertexIndex] = glm::vec2{lerpByIndex, 2.0f / 3.0f};
      }
      // If pole is the lower one
      else {
        uvs[currVertexIndex] = glm::vec2{1.0f - lerpByIndex, 1.0f / 3.0f};
      }
    }

    const int poleIndex = currPoleOffset + 5;

    // Creating pole vertex
    vertices[poleIndex] = glm::vec3{0.0f, 0.0f, 0.0f};

    // Setting UVs of the pole vertex
    // If pole is the upper one
    if (p == 0) {
      uvs[poleIndex] = glm::vec2{0.0f, 1.0f};
    }
    // If pole is the lower one
    else {
      uvs[poleIndex] = glm::vec2{0.0f, 0.0f};
    }

    // For each vertex of the hemisphere of appropriate pole except the pole vertex
    for (int v = 0; v < 5; ++v) {
      const int currVertexIndex = currPoleOffset + v;
      const int nextVertexIndex = currPoleOffset + (v + 1) % 5;
      const int indexOffset     = (currPoleOffset - p + v) * 3;

      // Configuring triangles of the hemisphere
      indices[indexOffset]     = currVertexIndex;
      indices[indexOffset + 1] = poleIndex;
      indices[indexOffset + 2] = nextVertexIndex;
    }

    // Translating and rotating hemisphere vertices
    // If pole is the upper one
    if (p == 0) {
      // For each vertex of the hemisphere of appropriate pole including the pole vertex
      for (int v = 0; v < 6; ++v) {
        vertices[v].y += 0.5f;
        vertices[v] = glm::angleAxis(glm::radians(-144.0f), kUp) * vertices[v];
      }
    }
    // If pole is the lower one
    else {
      // For each vertex of the hemisphere of appropriate pole including the pole vertex
      for (int v = 0; v < 6; ++v) {
        vertices[6 + v].y *= -1.0f;
        vertices[6 + v].y -= 0.5f;
        vertices[6 + v].x *= -1.0f;
      }
    }

    // Setting tangents of vertices of the hemisphere of appropriate pole including the pole vertex
    // For each vertex of the hemisphere of appropriate pole except the pole vertex
    for (int v = 0; v < 5; ++v) {
      const int currVertexIndex = currPoleOffset + v;
      const int nextVertexIndex = currPoleOffset + (v + 1) % 5;

      tangents[currVertexIndex] = calculateTangent(
          std::vector<glm::vec3>{vertices[poleIndex], vertices[currVertexIndex],
                                 vertices[nextVertexIndex]},
          std::vector<glm::vec2>{uvs[poleIndex], uvs[currVertexIndex], uvs[nextVertexIndex]});
    }
    tangents[poleIndex] = calculateTangent(
        std::vector<glm::vec3>{vertices[poleIndex], vertices[currPoleOffset],
                               vertices[currPoleOffset + 1]},
        std::vector<glm::vec2>{uvs[poleIndex], uvs[currPoleOffset], uvs[currPoleOffset + 1]});
  }

  // For each pair of vertices except pole vertices
  for (int v = 0; v < 5; ++v) {
    const int currUpperVertexIndex = v;
    const int nextUpperVertexIndex = (v + 1) % 5;
    const int currLowerVertexIndex = 6 + (5 - v) % 5;
    const int nextLowerVertexIndex = 6 + (10 - (v + 1)) % 5;
    const int indexOffset          = 30 + v * 6;

    // Configuring triangles between hemispheres
    indices[indexOffset]     = currUpperVertexIndex;
    indices[indexOffset + 1] = nextUpperVertexIndex;
    indices[indexOffset + 2] = currLowerVertexIndex;
    indices[indexOffset + 3] = nextUpperVertexIndex;
    indices[indexOffset + 4] = nextLowerVertexIndex;
    indices[indexOffset + 5] = currLowerVertexIndex;
  }

  // Projecting the icosahedron on a sphere
  for (int v = 0; v < kVertexCount; ++v) {
    normals[v]  = glm::normalize(vertices[v]);
    vertices[v] = normals[v] * radius;
  }

  // Generating vertex buffer based on vertices, normals, tangents and uvs
  std::vector<float> vertexBuffer{glengine::generateVertexBuffer(vertices, normals, tangents, uvs)};

  // Configuring VBO attributes
  std::vector<VBOAttribute> vboAttributes{};
  constexpr int             kOffset = 11;
  vboAttributes.push_back(
      VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float), reinterpret_cast<void *>(0)});
  vboAttributes.push_back(VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                       reinterpret_cast<void *>(3 * sizeof(float))});
  vboAttributes.push_back(VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                       reinterpret_cast<void *>(6 * sizeof(float))});
  vboAttributes.push_back(VBOAttribute{2, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                       reinterpret_cast<void *>(9 * sizeof(float))});

  // Generating and returning the mesh
  return generateMesh(vboAttributes, vertexBuffer, indices, shaderProgram, textures);
}
