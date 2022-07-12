// Header file
#include "./Mesh.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Mesh::Material::Texture::Texture() noexcept {}

// Parameterized constructor
Mesh::Material::Texture::Texture(GLuint name, int unit, bool isCubemap) noexcept
    : _name{name}, _unit{unit}, _isCubemap{isCubemap} {}

// Copy constructor
Mesh::Material::Texture::Texture(const Texture &texture) noexcept
    : _name{texture._name}, _unit{texture._unit}, _isCubemap{texture._isCubemap} {}

// Copy assignment operator
Mesh::Material::Texture &Mesh::Material::Texture::operator=(const Texture &texture) noexcept {
  _name      = texture._name;
  _unit      = texture._unit;
  _isCubemap = texture._isCubemap;

  return *this;
}

// Move constructor
Mesh::Material::Texture::Texture(Texture &&texture) noexcept
    : _name{std::exchange(texture._name, 0)},
      _unit{std::exchange(texture._unit, 0)},
      _isCubemap{std::exchange(texture._isCubemap, false)} {}

// Move assignment operator
Mesh::Material::Texture &Mesh::Material::Texture::operator=(Texture &&texture) noexcept {
  std::swap(_name, texture._name);
  std::swap(_unit, texture._unit);
  std::swap(_isCubemap, texture._isCubemap);

  return *this;
}

// Destructor
Mesh::Material::Texture::~Texture() noexcept {
  if (_name > 0) {
    glDeleteTextures(1, &_name);
  }
}

// Setters

void Mesh::Material::Texture::setName(GLuint name) noexcept { _name = name; }

void Mesh::Material::Texture::setUnit(int unit) noexcept { _unit = unit; }

void Mesh::Material::Texture::setIsCubemap(bool isCubemap) noexcept { _isCubemap = isCubemap; }

// Getters

GLuint Mesh::Material::Texture::getName() const noexcept { return _name; }

GLuint &Mesh::Material::Texture::getName() noexcept { return _name; }

int Mesh::Material::Texture::getUnit() const noexcept { return _unit; }

int &Mesh::Material::Texture::getUnit() noexcept { return _unit; }

bool Mesh::Material::Texture::getIsCubemap() const noexcept { return _isCubemap; }

bool &Mesh::Material::Texture::getIsCubemap() noexcept { return _isCubemap; }
