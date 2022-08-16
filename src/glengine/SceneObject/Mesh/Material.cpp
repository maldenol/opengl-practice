// Header file
#include "./Mesh.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
Mesh::Material::Material() noexcept {}

// Parameterized constructor
Mesh::Material::Material(const std::vector<std::shared_ptr<Texture>> &texturePtrs,
                         float                                        parallaxStrength) noexcept
    : _texturePtrs{texturePtrs}, _parallaxStrength{parallaxStrength} {}

// Copy constructor
Mesh::Material::Material(const Material &material) noexcept
    : _texturePtrs{material._texturePtrs}, _parallaxStrength{material._parallaxStrength} {}

// Copy assignment operator
Mesh::Material &Mesh::Material::operator=(const Material &material) noexcept {
  _texturePtrs      = material._texturePtrs;
  _parallaxStrength = material._parallaxStrength;

  return *this;
}

// Move constructor
Mesh::Material::Material(Material &&material) noexcept
    : _texturePtrs{std::exchange(material._texturePtrs, std::vector<std::shared_ptr<Texture>>{})},
      _parallaxStrength{std::exchange(material._parallaxStrength, 0.0f)} {}

// Move assignment operator
Mesh::Material &Mesh::Material::operator=(Material &&material) noexcept {
  std::swap(_texturePtrs, material._texturePtrs);
  std::swap(_parallaxStrength, material._parallaxStrength);

  return *this;
}

// Destructor
Mesh::Material::~Material() noexcept {}

// Setters

void Mesh::Material::setTexturePtrs(
    const std::vector<std::shared_ptr<Texture>> &texturePtrs) noexcept {
  _texturePtrs = texturePtrs;
}

void Mesh::Material::setParallaxStrength(float parallaxStrength) noexcept {
  _parallaxStrength = parallaxStrength;
}

// Getters

const std::vector<std::shared_ptr<Mesh::Material::Texture>> &Mesh::Material::getTexturePtrs()
    const noexcept {
  return _texturePtrs;
}

std::vector<std::shared_ptr<Mesh::Material::Texture>> &Mesh::Material::getTexturePtrs() noexcept {
  return _texturePtrs;
}

float Mesh::Material::getParallaxStrength() const noexcept { return _parallaxStrength; }

float &Mesh::Material::getParallaxStrength() noexcept { return _parallaxStrength; }
