// Header file
#include "../Mesh.hpp"

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// Generates quad sphere mesh based on radius, level-of-detail, enableCubemap, shader program and textures
glengine::Mesh glengine::generateQuadSphere(
    float radius, unsigned int lod, bool enableCubemap, GLuint shaderProgram,
    const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs) {
  // Level-of-detail (count of quads along one side)
  const float xyQuadSize = 1.0f / static_cast<float>(lod);  // discrete quad's side xy size
  const float uQuadSize  = 1.0f / (enableCubemap ? 4.0f : 1.0f) /
                          static_cast<float>(lod);  // discrete quad's side u size
  const float vQuadSize = 1.0f / (enableCubemap ? 3.0f : 1.0f) /
                          static_cast<float>(lod);  // discrete quad's side v size
  const float        halfSize = 1.0f / 2.0f;        // half of the size
  const unsigned int quadLOD  = lod * lod;          // LOD^2

  const unsigned int quadPerSideCount   = quadLOD;                 // discrete quads per side count
  const unsigned int vertexPerSideCount = 4 * quadPerSideCount;    // vertices per side count
  const unsigned int vertexCount        = 6 * vertexPerSideCount;  // vertices count
  const unsigned int indexPerSideCount  = 2 * quadPerSideCount * 3;  // 3 indexes for each triangle
  const unsigned int indexCount         = 6 * indexPerSideCount;     // 3 indexes for each triangle

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

  // For each side
  for (unsigned int s = 0; s < 6; ++s) {
    // For each quad of the side
    for (unsigned int v = 0; v < quadPerSideCount; ++v) {
      const unsigned int row    = v / lod;
      const unsigned int column = v % lod;
      const float        leftX  = static_cast<float>(column) * xyQuadSize - halfSize;
      const float        rightX = static_cast<float>(column + 1) * xyQuadSize - halfSize;
      const float        downY  = static_cast<float>(row) * xyQuadSize - halfSize;
      const float        upY    = static_cast<float>(row + 1) * xyQuadSize - halfSize;
      const float        leftU  = static_cast<float>(column) * uQuadSize;
      const float        rightU = static_cast<float>(column + 1) * uQuadSize;
      const float        downV  = static_cast<float>(row) * vQuadSize;
      const float        upV    = static_cast<float>(row + 1) * vQuadSize;

      glm::vec3 lu{leftX, upY, 0.0f};
      glm::vec3 ru{rightX, upY, 0.0f};
      glm::vec3 ld{leftX, downY, 0.0f};
      glm::vec3 rd{rightX, downY, 0.0f};

      glm::vec3 n{-kForward};

      glm::vec2 luUV{leftU, upV};
      glm::vec2 ruUV{rightU, upV};
      glm::vec2 ldUV{leftU, downV};
      glm::vec2 rdUV{rightU, downV};

      // Placing the side on its place
      switch (s) {
        case 0:  // x-
          lu = glm::angleAxis(glm::radians(-90.0f), kUp) * lu;
          ru = glm::angleAxis(glm::radians(-90.0f), kUp) * ru;
          ld = glm::angleAxis(glm::radians(-90.0f), kUp) * ld;
          rd = glm::angleAxis(glm::radians(-90.0f), kUp) * rd;

          n = glm::angleAxis(glm::radians(-90.0f), kUp) * n;

          lu.x -= halfSize;
          ru.x -= halfSize;
          ld.x -= halfSize;
          rd.x -= halfSize;

          if (enableCubemap) {
            luUV.x += 0.0f / 4.0f;
            luUV.y += 1.0f / 3.0f;
            ruUV.x += 0.0f / 4.0f;
            ruUV.y += 1.0f / 3.0f;
            ldUV.x += 0.0f / 4.0f;
            ldUV.y += 1.0f / 3.0f;
            rdUV.x += 0.0f / 4.0f;
            rdUV.y += 1.0f / 3.0f;
          }
          break;
        case 1:  // x+
          lu = glm::angleAxis(glm::radians(90.0f), kUp) * lu;
          ru = glm::angleAxis(glm::radians(90.0f), kUp) * ru;
          ld = glm::angleAxis(glm::radians(90.0f), kUp) * ld;
          rd = glm::angleAxis(glm::radians(90.0f), kUp) * rd;

          n = glm::angleAxis(glm::radians(90.0f), kUp) * n;

          lu.x += halfSize;
          ru.x += halfSize;
          ld.x += halfSize;
          rd.x += halfSize;

          if (enableCubemap) {
            luUV.x += 2.0f / 4.0f;
            luUV.y += 1.0f / 3.0f;
            ruUV.x += 2.0f / 4.0f;
            ruUV.y += 1.0f / 3.0f;
            ldUV.x += 2.0f / 4.0f;
            ldUV.y += 1.0f / 3.0f;
            rdUV.x += 2.0f / 4.0f;
            rdUV.y += 1.0f / 3.0f;
          }
          break;
        case 2:  // y-
          lu = glm::angleAxis(glm::radians(90.0f), kRight) * lu;
          ru = glm::angleAxis(glm::radians(90.0f), kRight) * ru;
          ld = glm::angleAxis(glm::radians(90.0f), kRight) * ld;
          rd = glm::angleAxis(glm::radians(90.0f), kRight) * rd;

          n = glm::angleAxis(glm::radians(90.0f), kRight) * n;

          lu.y -= halfSize;
          ru.y -= halfSize;
          ld.y -= halfSize;
          rd.y -= halfSize;

          if (enableCubemap) {
            luUV.x += 1.0f / 4.0f;
            luUV.y += 0.0f / 3.0f;
            ruUV.x += 1.0f / 4.0f;
            ruUV.y += 0.0f / 3.0f;
            ldUV.x += 1.0f / 4.0f;
            ldUV.y += 0.0f / 3.0f;
            rdUV.x += 1.0f / 4.0f;
            rdUV.y += 0.0f / 3.0f;
          }
          break;
        case 3:  // y+
          lu = glm::angleAxis(glm::radians(-90.0f), kRight) * lu;
          ru = glm::angleAxis(glm::radians(-90.0f), kRight) * ru;
          ld = glm::angleAxis(glm::radians(-90.0f), kRight) * ld;
          rd = glm::angleAxis(glm::radians(-90.0f), kRight) * rd;

          n = glm::angleAxis(glm::radians(-90.0f), kRight) * n;

          lu.y += halfSize;
          ru.y += halfSize;
          ld.y += halfSize;
          rd.y += halfSize;

          if (enableCubemap) {
            luUV.x += 1.0f / 4.0f;
            luUV.y += 2.0f / 3.0f;
            ruUV.x += 1.0f / 4.0f;
            ruUV.y += 2.0f / 3.0f;
            ldUV.x += 1.0f / 4.0f;
            ldUV.y += 2.0f / 3.0f;
            rdUV.x += 1.0f / 4.0f;
            rdUV.y += 2.0f / 3.0f;
          }
          break;
        case 4:  // z-
          lu = glm::angleAxis(glm::radians(180.0f), kUp) * lu;
          ru = glm::angleAxis(glm::radians(180.0f), kUp) * ru;
          ld = glm::angleAxis(glm::radians(180.0f), kUp) * ld;
          rd = glm::angleAxis(glm::radians(180.0f), kUp) * rd;

          n = glm::angleAxis(glm::radians(180.0f), kUp) * n;

          lu.z -= halfSize;
          ru.z -= halfSize;
          ld.z -= halfSize;
          rd.z -= halfSize;

          if (enableCubemap) {
            luUV.x += 3.0f / 4.0f;
            luUV.y += 1.0f / 3.0f;
            ruUV.x += 3.0f / 4.0f;
            ruUV.y += 1.0f / 3.0f;
            ldUV.x += 3.0f / 4.0f;
            ldUV.y += 1.0f / 3.0f;
            rdUV.x += 3.0f / 4.0f;
            rdUV.y += 1.0f / 3.0f;
          }
          break;
        case 5:  // z+
          lu.z += halfSize;
          ru.z += halfSize;
          ld.z += halfSize;
          rd.z += halfSize;

          if (enableCubemap) {
            luUV.x += 1.0f / 4.0f;
            luUV.y += 1.0f / 3.0f;
            ruUV.x += 1.0f / 4.0f;
            ruUV.y += 1.0f / 3.0f;
            ldUV.x += 1.0f / 4.0f;
            ldUV.y += 1.0f / 3.0f;
            rdUV.x += 1.0f / 4.0f;
            rdUV.y += 1.0f / 3.0f;
          }
          break;
      }

      const unsigned int vertexOffset = s * vertexPerSideCount + v * 4;
      const unsigned int indexOffset  = s * quadLOD * 6 + v * 6;

      vertices[vertexOffset]     = lu;
      vertices[vertexOffset + 1] = ru;
      vertices[vertexOffset + 2] = ld;
      vertices[vertexOffset + 3] = rd;

      normals[vertexOffset]     = n;
      normals[vertexOffset + 1] = n;
      normals[vertexOffset + 2] = n;
      normals[vertexOffset + 3] = n;

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

      indices[indexOffset]     = vertexOffset;      // top-right triangle
      indices[indexOffset + 1] = vertexOffset + 2;  // top-right triangle
      indices[indexOffset + 2] = vertexOffset + 1;  // top-right triangle
      indices[indexOffset + 3] = vertexOffset + 1;  // bottom-left triangle
      indices[indexOffset + 4] = vertexOffset + 2;  // bottom-left triangle
      indices[indexOffset + 5] = vertexOffset + 3;  // bottom-left triangle
    }
  }

  // Projecting the cube on a sphere
  for (unsigned int v = 0; v < vertexCount; ++v) {
    normals[v]  = glm::normalize(vertices[v]);
    vertices[v] = normals[v] * radius;
  }

  // Generating vertex buffer based on vertices, normals, tangents and uvs
  std::vector<float> vertexBuffer{generateVertexBuffer(vertices, normals, tangents, uvs)};

  // Configuring VBO attributes
  std::vector<Mesh::VBOAttribute> vboAttributes{};
  constexpr unsigned int          kOffset = 11;
  vboAttributes.push_back(Mesh::VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(0)});
  vboAttributes.push_back(Mesh::VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(3 * sizeof(float))});
  vboAttributes.push_back(Mesh::VBOAttribute{3, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(6 * sizeof(float))});
  vboAttributes.push_back(Mesh::VBOAttribute{2, GL_FLOAT, GL_FALSE, kOffset * sizeof(float),
                                             reinterpret_cast<void *>(9 * sizeof(float))});

  // Creating and returning the mesh
  return Mesh{vboAttributes, vertexBuffer, indices, shaderProgram,
              std::make_shared<Mesh::Material>(Mesh::Material{texturePtrs})};
}
