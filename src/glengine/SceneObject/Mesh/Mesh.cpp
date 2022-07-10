// Header file
#include "./Mesh.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Mesh::Mesh() noexcept {}

// Parameterized constructor
Mesh::Mesh(GLuint vao, GLuint vbo, GLuint ebo, GLsizei indexCount, GLuint shaderProgram,
           const std::shared_ptr<Material> &materialPtr) noexcept
    : _vao{vao},
      _vbo{vbo},
      _ebo{ebo},
      _indexCount{indexCount},
      _shaderProgram{shaderProgram},
      _materialPtr{materialPtr} {}

// Parameterized constructor
Mesh::Mesh(const std::vector<VBOAttribute> &vboAttributes, const std::vector<float> &vertexBuffer,
           const std::vector<GLuint> &indices, GLuint shaderProgram,
           const std::shared_ptr<Material> &materialPtr)
    : _indexCount{static_cast<GLsizei>(indices.size())},
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
      _shaderProgram{mesh._shaderProgram},
      _materialPtr{mesh._materialPtr} {}

// Copy assignment operator
Mesh &Mesh::operator=(const Mesh &mesh) noexcept {
  _vao           = mesh._vao;
  _vbo           = mesh._vbo;
  _ebo           = mesh._ebo;
  _indexCount    = mesh._indexCount;
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
      _shaderProgram{std::exchange(mesh._shaderProgram, 0)},
      _materialPtr{std::exchange(mesh._materialPtr, std::shared_ptr<Material>{})} {}

// Move assignment operator
Mesh &Mesh::operator=(Mesh &&mesh) noexcept {
  std::swap(_vao, mesh._vao);
  std::swap(_vbo, mesh._vbo);
  std::swap(_ebo, mesh._ebo);
  std::swap(_indexCount, mesh._indexCount);
  std::swap(_shaderProgram, mesh._shaderProgram);
  std::swap(_materialPtr, mesh._materialPtr);

  return *this;
}

// Destructor
Mesh::~Mesh() noexcept {
  if (_vao > 0) {
    glDeleteVertexArrays(1, &_vao);
  }
  if (_vbo > 0) {
    glDeleteBuffers(1, &_vbo);
  }
  if (_ebo > 0) {
    glDeleteBuffers(1, &_ebo);
  }
  if (_shaderProgram > 0) {
    glDeleteProgram(_shaderProgram);
  }
}

// Setters

void Mesh::setVAO(GLuint vao) noexcept { _vao = vao; }

void Mesh::setVBO(GLuint vbo) noexcept { _vbo = vbo; }

void Mesh::setEBO(GLuint ebo) noexcept { _ebo = ebo; }

void Mesh::setIndexCount(GLsizei indexCount) noexcept { _indexCount = indexCount; }

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

GLuint Mesh::getShaderProgram() const noexcept { return _shaderProgram; }

GLuint &Mesh::getShaderProgram() noexcept { return _shaderProgram; }

const std::shared_ptr<Mesh::Material> &Mesh::getMaterialPtr() const noexcept {
  return _materialPtr;
}

std::shared_ptr<Mesh::Material> &Mesh::getMaterialPtr() noexcept { return _materialPtr; }

void Mesh::render(unsigned int instanceCount) const noexcept {
  if (!isComplete()) return;

  const Mesh::Material &material = *_materialPtr;

  // Binding VAO with associated VBO and EBO
  glBindVertexArray(_vao);

  // For each texture
  for (size_t i = 0; i < material.texturePtrs.size(); ++i) {
    // Binding texture to texture unit
    glActiveTexture(GL_TEXTURE0 + material.texturePtrs[i]->index);
    if (material.texturePtrs[i]->isCubemap) {
      glBindTexture(GL_TEXTURE_CUBE_MAP, material.texturePtrs[i]->texture);
    } else {
      glBindTexture(GL_TEXTURE_2D, material.texturePtrs[i]->texture);
    }
  }

  // Setting specific shader program to use for render
  glUseProgram(_shaderProgram);

  // Drawing mesh
  glDrawElementsInstanced(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);

  // Unbinding shader program
  glUseProgram(0);

  // For each texture
  for (size_t i = 0; i < material.texturePtrs.size(); ++i) {
    // Unbinding texture from texture unit
    glActiveTexture(GL_TEXTURE0 + material.texturePtrs[i]->index);
    if (material.texturePtrs[i]->isCubemap) {
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    } else {
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

  // Unbinding VAO
  glBindVertexArray(0);
}

bool Mesh::isComplete() const noexcept {
  return _vao > 0 && _vbo > 0 && _ebo > 0 && _indexCount > 0 && _shaderProgram > 0 &&
         _materialPtr != nullptr;
}
