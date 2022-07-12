// Header file
#include "./Mesh.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Mesh::Material::Material() noexcept {}

// Parameterized constructor
Mesh::Material::Material(float ambCoef, float diffCoef, float specCoef, float glossiness,
                         float                                        maxHeight,
                         const std::vector<std::shared_ptr<Texture>> &texturePtrs) noexcept
    : _ambCoef{ambCoef},
      _diffCoef{diffCoef},
      _specCoef{specCoef},
      _glossiness{glossiness},
      _maxHeight{maxHeight},
      _texturePtrs{texturePtrs} {}

// Copy constructor
Mesh::Material::Material(const Material &material) noexcept
    : _ambCoef{material._ambCoef},
      _diffCoef{material._diffCoef},
      _specCoef{material._specCoef},
      _glossiness{material._glossiness},
      _maxHeight{material._maxHeight},
      _texturePtrs{material._texturePtrs} {}

// Copy assignment operator
Mesh::Material &Mesh::Material::operator=(const Material &material) noexcept {
  _ambCoef     = material._ambCoef;
  _diffCoef    = material._diffCoef;
  _specCoef    = material._specCoef;
  _glossiness  = material._glossiness;
  _maxHeight   = material._maxHeight;
  _texturePtrs = material._texturePtrs;

  return *this;
}

// Move constructor
Mesh::Material::Material(Material &&material) noexcept
    : _ambCoef{std::exchange(material._ambCoef, 0.0f)},
      _diffCoef{std::exchange(material._diffCoef, 0.0f)},
      _specCoef{std::exchange(material._specCoef, 0.0f)},
      _glossiness{std::exchange(material._glossiness, 0.0f)},
      _maxHeight{std::exchange(material._maxHeight, 0.0f)},
      _texturePtrs{std::exchange(material._texturePtrs, std::vector<std::shared_ptr<Texture>>{})} {}

// Move assignment operator
Mesh::Material &Mesh::Material::operator=(Material &&material) noexcept {
  std::swap(_ambCoef, material._ambCoef);
  std::swap(_diffCoef, material._diffCoef);
  std::swap(_specCoef, material._specCoef);
  std::swap(_glossiness, material._glossiness);
  std::swap(_maxHeight, material._maxHeight);
  std::swap(_texturePtrs, material._texturePtrs);

  return *this;
}

// Destructor
Mesh::Material::~Material() noexcept {}

// Setters

void Mesh::Material::setAmbCoef(float ambCoef) noexcept { _ambCoef = ambCoef; }

void Mesh::Material::setDiffCoef(float diffCoef) noexcept { _diffCoef = diffCoef; }

void Mesh::Material::setSpecCoef(float specCoef) noexcept { _specCoef = specCoef; }

void Mesh::Material::setGlossiness(float glossiness) noexcept { _glossiness = glossiness; }

void Mesh::Material::setMaxHeight(float maxHeight) noexcept { _maxHeight = maxHeight; }

void Mesh::Material::setTexturePtrs(
    const std::vector<std::shared_ptr<Texture>> &texturePtrs) noexcept {
  _texturePtrs = texturePtrs;
}

// Getters

float Mesh::Material::getAmbCoef() const noexcept { return _ambCoef; }

float &Mesh::Material::getAmbCoef() noexcept { return _ambCoef; }

float Mesh::Material::getDiffCoef() const noexcept { return _diffCoef; }

float &Mesh::Material::getDiffCoef() noexcept { return _diffCoef; }

float Mesh::Material::getSpecCoef() const noexcept { return _specCoef; }

float &Mesh::Material::getSpecCoef() noexcept { return _specCoef; }

float Mesh::Material::getGlossiness() const noexcept { return _glossiness; }

float &Mesh::Material::getGlossiness() noexcept { return _glossiness; }

float Mesh::Material::getMaxHeight() const noexcept { return _maxHeight; }

float &Mesh::Material::getMaxHeight() noexcept { return _maxHeight; }

const std::vector<std::shared_ptr<Mesh::Material::Texture>> &Mesh::Material::getTexturePtrs()
    const noexcept {
  return _texturePtrs;
}

std::vector<std::shared_ptr<Mesh::Material::Texture>> &Mesh::Material::getTexturePtrs() noexcept {
  return _texturePtrs;
}
