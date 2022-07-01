// Header file
#include "./mesh.hpp"

// Generates plane mesh based on size, level-of-detail, shader program and textures
glservice::Mesh glservice::generatePlane(float size, int lod, GLuint shaderProgram,
                                         const std::vector<Texture> &textures) {
  // Level-Of-Details (count of quads along one side)
  const float xyQuadSize =
      static_cast<float>(size) / static_cast<float>(lod);    // discrete quad's side xy size
  const float uvQuadSize = 1.0f / static_cast<float>(lod);   // discrete quad's side uv size
  const float halfSize   = static_cast<float>(size) / 2.0f;  // half of the SIZE
  const int   quadLOD    = lod * lod;                        // LOD^2

  const int quadCount   = quadLOD;            // count of discrete quads
  const int vertexCount = 4 * quadCount;      // count of vertices
  const int indexCount  = 2 * quadCount * 3;  // 3 indexes for each triangle

  std::vector<glm::vec3> vertices{};
  std::vector<glm::vec3> normals{};
  std::vector<glm::vec2> uvs{};
  std::vector<GLuint>    indices{};
  vertices.resize(vertexCount);
  normals.resize(vertexCount);
  uvs.resize(vertexCount);
  indices.resize(indexCount);

  for (int v = 0; v < quadCount; ++v) {
    const int row    = v / lod;
    const int column = v % lod;

    const float lux = column * xyQuadSize - halfSize;
    const float rux = (column + 1) * xyQuadSize - halfSize;
    const float ldx = lux;
    const float rdx = rux;
    const float luy = (row + 1) * xyQuadSize - halfSize;
    const float ruy = luy;
    const float ldy = row * xyQuadSize - halfSize;
    const float rdy = ldy;
    const float luu = column * uvQuadSize;
    const float ruu = (column + 1) * uvQuadSize;
    const float ldu = luu;
    const float rdu = ruu;
    const float luv = (row + 1) * uvQuadSize;
    const float ruv = luv;
    const float ldv = row * uvQuadSize;
    const float rdv = ldv;

    constexpr glm::vec3 kForward{0.0f, 0.0f, 1.0f};

    const int vertexOffset = v * 4;
    const int indexOffset  = v * 6;

    vertices[vertexOffset]     = glm::vec3{lux, luy, 0.0f};  // left-up
    vertices[vertexOffset + 1] = glm::vec3{rux, ruy, 0.0f};  // right-up
    vertices[vertexOffset + 2] = glm::vec3{ldx, ldy, 0.0f};  // left-down
    vertices[vertexOffset + 3] = glm::vec3{rdx, rdy, 0.0f};  // right-down

    normals[vertexOffset]     = -kForward;  // left-up
    normals[vertexOffset + 1] = -kForward;  // right-up
    normals[vertexOffset + 2] = -kForward;  // left-down
    normals[vertexOffset + 3] = -kForward;  // right-down

    uvs[vertexOffset]     = glm::vec2{luu, luv};  // left-up
    uvs[vertexOffset + 1] = glm::vec2{ruu, ruv};  // right-up
    uvs[vertexOffset + 2] = glm::vec2{ldu, ldv};  // left-down
    uvs[vertexOffset + 3] = glm::vec2{rdu, rdv};  // right-down

    indices[indexOffset]     = vertexOffset;      // left-up triangle
    indices[indexOffset + 1] = vertexOffset + 1;  // left-up triangle
    indices[indexOffset + 2] = vertexOffset + 2;  // left-up triangle
    indices[indexOffset + 3] = vertexOffset + 1;  // right-down triangle
    indices[indexOffset + 4] = vertexOffset + 3;  // right-down triangle
    indices[indexOffset + 5] = vertexOffset + 2;  // right-down triangle
  }

  // Generating vertex buffer based on vertices, normals and uvs
  std::vector<float> vertexBuffer = glservice::generateVertexBuffer(vertices, normals, uvs);

  // Configuring VBO attributes
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

  // Generating and returning the mesh
  return generateMesh(vboAttributes, vertexBuffer, indices, shaderProgram, textures);
}
