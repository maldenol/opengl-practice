// Header file
#include "../Mesh.hpp"

// Generates plane mesh based on size, level-of-detail, shader program and textures
glengine::Mesh glengine::generatePlane(
    float size, unsigned int lod, GLuint shaderProgram,
    const std::vector<std::shared_ptr<Mesh::Material::Texture>> &texturePtrs) {
  // Level-of-detail (count of quads along one side)
  const float xyQuadSize =
      static_cast<float>(size) / static_cast<float>(lod);           // discrete quad's side xy size
  const float        uvQuadSize = 1.0f / static_cast<float>(lod);   // discrete quad's side uv size
  const float        halfSize   = static_cast<float>(size) / 2.0f;  // half of the SIZE
  const unsigned int quadLOD    = lod * lod;                        // LOD^2

  const unsigned int quadCount   = quadLOD;            // count of discrete quads
  const unsigned int vertexCount = 4 * quadCount;      // count of vertices
  const unsigned int indexCount  = 2 * quadCount * 3;  // 3 indexes for each triangle

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

  // For each quad
  for (unsigned int v = 0; v < quadCount; ++v) {
    const unsigned int row    = v / lod;
    const unsigned int column = v % lod;

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

    constexpr glm::vec3 kForward{0.0f, 0.0f, -1.0f};

    const glm::vec3 lu{lux, luy, 0.0f};
    const glm::vec3 ru{rux, ruy, 0.0f};
    const glm::vec3 ld{ldx, ldy, 0.0f};
    const glm::vec3 rd{rdx, rdy, 0.0f};

    const glm::vec3 n{-kForward};

    const glm::vec2 luUV{luu, luv};
    const glm::vec2 ruUV{ruu, ruv};
    const glm::vec2 ldUV{ldu, ldv};
    const glm::vec2 rdUV{rdu, rdv};

    const unsigned int vertexOffset = v * 4;
    const unsigned int indexOffset  = v * 6;

    vertices[vertexOffset]     = lu;  // top-left
    vertices[vertexOffset + 1] = ru;  // top-right
    vertices[vertexOffset + 2] = ld;  // bottom-left
    vertices[vertexOffset + 3] = rd;  // bottom-right

    normals[vertexOffset]     = n;  // top-left
    normals[vertexOffset + 1] = n;  // top-right
    normals[vertexOffset + 2] = n;  // bottom-left
    normals[vertexOffset + 3] = n;  // bottom-right

    tangents[vertexOffset]     = calculateTangent(  // top-left
        std::vector<glm::vec3>{lu, ld, rd}, std::vector<glm::vec2>{luUV, ldUV, rdUV});
    tangents[vertexOffset + 1] = calculateTangent(  // top-right
        std::vector<glm::vec3>{ld, rd, ru}, std::vector<glm::vec2>{ldUV, rdUV, ruUV});
    tangents[vertexOffset + 2] = calculateTangent(  // bottom-left
        std::vector<glm::vec3>{rd, ru, lu}, std::vector<glm::vec2>{rdUV, ruUV, luUV});
    tangents[vertexOffset + 3] = calculateTangent(  // bottom-right
        std::vector<glm::vec3>{ru, lu, ld}, std::vector<glm::vec2>{ruUV, luUV, ldUV});

    uvs[vertexOffset]     = luUV;  // top-left
    uvs[vertexOffset + 1] = ruUV;  // top-right
    uvs[vertexOffset + 2] = ldUV;  // bottom-left
    uvs[vertexOffset + 3] = rdUV;  // bottom-right

    indices[indexOffset]     = vertexOffset;
    indices[indexOffset + 1] = vertexOffset + 2;  // top-right triangle
    indices[indexOffset + 2] = vertexOffset + 1;  // top-right triangle
    indices[indexOffset + 3] = vertexOffset + 1;  // bottom-left triangle
    indices[indexOffset + 4] = vertexOffset + 2;  // bottom-left triangle
    indices[indexOffset + 5] = vertexOffset + 3;  // bottom-left triangle
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
              std::make_shared<Mesh::Material>(Mesh::Material{kInitAmbCoef, kInitDiffCoef,
                                                              kInitSpecCoef, kInitGlossiness,
                                                              kInitMaxHeight, texturePtrs})};
}
