// Header file
#include "./Mesh.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Mesh::Mesh() noexcept {}

// Parameterized constructor
Mesh::Mesh(GLuint vao, GLuint vbo, GLuint ebo, GLsizei indexCount, GLsizei instanceCount,
           GLuint shaderProgram, const std::shared_ptr<Material> &materialPtr) noexcept
    : _vao{vao},
      _vbo{vbo},
      _ebo{ebo},
      _indexCount{indexCount},
      _instanceCount{instanceCount},
      _shaderProgram{shaderProgram},
      _materialPtr{materialPtr} {}

// Parameterized constructor
Mesh::Mesh(const std::vector<VBOAttribute> &vboAttributes, const std::vector<float> &vertexBuffer,
           const std::vector<GLuint> &indices, GLuint shaderProgram,
           const std::shared_ptr<Material> &materialPtr)
    : _indexCount{static_cast<GLsizei>(indices.size())},
      _instanceCount{1},
      _shaderProgram{shaderProgram},
      _materialPtr{materialPtr} {
  // Creating VAO, VBO and EBO
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  // Binding VAO to bind to it vertex attributes and EBO and then configure them
  glBindVertexArray(_vao);

  // Binding and filling VBO
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0],
               GL_STATIC_DRAW);

  // Binding and filling EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0],
               GL_STATIC_DRAW);

  // Configuring and enabling VBO's attributes
  for (size_t i = 0; i < vboAttributes.size(); ++i) {
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, vboAttributes[i].size, vboAttributes[i].type,
                          vboAttributes[i].normalized, vboAttributes[i].stride,
                          vboAttributes[i].pointer);
  }

  // Unbinding configured VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Copy constructor
Mesh::Mesh(const Mesh &mesh) noexcept
    : _vao{mesh._vao},
      _vbo{mesh._vbo},
      _ebo{mesh._ebo},
      _indexCount{mesh._indexCount},
      _instanceCount{mesh._instanceCount},
      _shaderProgram{mesh._shaderProgram},
      _materialPtr{mesh._materialPtr} {}

// Copy assignment operator
Mesh &Mesh::operator=(const Mesh &mesh) noexcept {
  _vao           = mesh._vao;
  _vbo           = mesh._vbo;
  _ebo           = mesh._ebo;
  _indexCount    = mesh._indexCount;
  _instanceCount = mesh._instanceCount;
  _shaderProgram = mesh._shaderProgram;
  _materialPtr   = mesh._materialPtr;

  return *this;
}

// Move constructor
Mesh::Mesh(Mesh &&mesh) noexcept
    : _vao{std::exchange(mesh._vao, 0)},
      _vbo{std::exchange(mesh._vbo, 0)},
      _ebo{std::exchange(mesh._ebo, 0)},
      _indexCount{std::exchange(mesh._indexCount, 0)},
      _instanceCount{std::exchange(mesh._instanceCount, 0)},
      _shaderProgram{std::exchange(mesh._shaderProgram, 0)},
      _materialPtr{std::exchange(mesh._materialPtr, std::shared_ptr<Material>{})} {}

// Move assignment operator
Mesh &Mesh::operator=(Mesh &&mesh) noexcept {
  std::swap(_vao, mesh._vao);
  std::swap(_vbo, mesh._vbo);
  std::swap(_ebo, mesh._ebo);
  std::swap(_indexCount, mesh._indexCount);
  std::swap(_instanceCount, mesh._instanceCount);
  std::swap(_shaderProgram, mesh._shaderProgram);
  std::swap(_materialPtr, mesh._materialPtr);

  return *this;
}

// Destructor
Mesh::~Mesh() noexcept {
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

// Setters

void Mesh::setVAO(GLuint vao) noexcept { _vao = vao; }

void Mesh::setVBO(GLuint vbo) noexcept { _vbo = vbo; }

void Mesh::setEBO(GLuint ebo) noexcept { _ebo = ebo; }

void Mesh::setIndexCount(GLsizei indexCount) noexcept { _indexCount = indexCount; }

void Mesh::setInstanceCount(GLsizei instanceCount) noexcept { _instanceCount = instanceCount; }

void Mesh::setShaderProgram(GLuint shaderProgram) noexcept { _shaderProgram = shaderProgram; }

void Mesh::setMaterialPtr(const std::shared_ptr<Material> &materialPtr) noexcept {
  _materialPtr = materialPtr;
}

// Getters

GLuint Mesh::getVAO() const noexcept { return _vao; }

GLuint &Mesh::getVAO() noexcept { return _vao; }

GLuint Mesh::getVBO() const noexcept { return _vbo; }

GLuint &Mesh::getVBO() noexcept { return _vbo; }

GLuint Mesh::getEBO() const noexcept { return _ebo; }

GLuint &Mesh::getEBO() noexcept { return _ebo; }

GLsizei Mesh::getIndexCount() const noexcept { return _indexCount; }

GLsizei &Mesh::getIndexCount() noexcept { return _indexCount; }

GLsizei Mesh::getInstanceCount() const noexcept { return _instanceCount; }

GLsizei &Mesh::getInstanceCount() noexcept { return _instanceCount; }

GLuint Mesh::getShaderProgram() const noexcept { return _shaderProgram; }

GLuint &Mesh::getShaderProgram() noexcept { return _shaderProgram; }

const std::shared_ptr<Mesh::Material> &Mesh::getMaterialPtr() const noexcept {
  return _materialPtr;
}

std::shared_ptr<Mesh::Material> &Mesh::getMaterialPtr() noexcept { return _materialPtr; }

// Other member functions

void Mesh::render() const noexcept {
  if (!isComplete()) return;

  const Mesh::Material &material = *_materialPtr;

  // Binding VAO with associated VBO and EBO
  glBindVertexArray(_vao);

  // For each texture
  for (size_t i = 0; i < material.getTexturePtrs().size(); ++i) {
    // Binding texture to texture unit
    glActiveTexture(GL_TEXTURE0 + material.getTexturePtrs()[i]->getUnit());
    if (material.getTexturePtrs()[i]->getIsCubemap()) {
      glBindTexture(GL_TEXTURE_CUBE_MAP, material.getTexturePtrs()[i]->getName());
    } else {
      glBindTexture(GL_TEXTURE_2D, material.getTexturePtrs()[i]->getName());
    }
  }

  // Setting specific shader program to use for render
  glUseProgram(_shaderProgram);

  // Updating object shader program uniform values
  glUniform1f(glGetUniformLocation(_shaderProgram, "MATERIAL.ambCoef"), _materialPtr->getAmbCoef());
  glUniform1f(glGetUniformLocation(_shaderProgram, "MATERIAL.diffCoef"),
              _materialPtr->getDiffCoef());
  glUniform1f(glGetUniformLocation(_shaderProgram, "MATERIAL.specCoef"),
              _materialPtr->getSpecCoef());
  glUniform1f(glGetUniformLocation(_shaderProgram, "MATERIAL.glossiness"),
              _materialPtr->getGlossiness());
  glUniform1f(glGetUniformLocation(_shaderProgram, "MATERIAL.maxHeight"),
              _materialPtr->getMaxHeight());
  glUniform1i(glGetUniformLocation(_shaderProgram, "MATERIAL.albedoMap"), 0);
  glUniform1i(glGetUniformLocation(_shaderProgram, "MATERIAL.normalMap"), 1);
  glUniform1i(glGetUniformLocation(_shaderProgram, "MATERIAL.heightMap"), 2);
  glUniform1i(glGetUniformLocation(_shaderProgram, "MATERIAL.ambOccMap"), 3);
  glUniform1i(glGetUniformLocation(_shaderProgram, "MATERIAL.roughMap"), 4);
  glUniform1i(glGetUniformLocation(_shaderProgram, "MATERIAL.emissMap"), 5);

  // If instanced
  glUniform1i(glGetUniformLocation(_shaderProgram, "INSTANCED"),
              static_cast<int>(_instanceCount > 1));

  // Drawing mesh
  glDrawElementsInstanced(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr, _instanceCount);

  // Unbinding shader program
  glUseProgram(0);

  // For each texture
  for (size_t i = 0; i < material.getTexturePtrs().size(); ++i) {
    // Unbinding texture from texture unit
    glActiveTexture(GL_TEXTURE0 + material.getTexturePtrs()[i]->getUnit());
    if (material.getTexturePtrs()[i]->getIsCubemap()) {
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    } else {
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

  // Unbinding VAO
  glBindVertexArray(0);
}

bool Mesh::isComplete() const noexcept {
  return _vao > 0 && _vbo > 0 && _ebo > 0 && _indexCount > 0 && _instanceCount > 0 &&
         _shaderProgram > 0 && _materialPtr != nullptr;
}
