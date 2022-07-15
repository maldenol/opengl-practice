// Header file
#include "./PointLight.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
PointLight::PointLight() noexcept {}

// Parameterized constructor
PointLight::PointLight(const glm::vec3 &color, float intensity, float linAttCoef,
                       float quadAttCoef) noexcept
    : BaseLight{color, intensity}, _linAttCoef{linAttCoef}, _quadAttCoef{quadAttCoef} {}

// Copy constructor (base class)
PointLight::PointLight(const BaseLight &light) noexcept : BaseLight{light} {}

// Copy assignment operator (base class)
PointLight &PointLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  return *this;
}

// Copy constructor
PointLight::PointLight(const PointLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)},
      _linAttCoef{light._linAttCoef},
      _quadAttCoef{light._quadAttCoef} {}

// Copy assignment operator
PointLight &PointLight::operator=(const PointLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  _linAttCoef  = light._linAttCoef;
  _quadAttCoef = light._quadAttCoef;

  return *this;
}

// Move constructor
PointLight::PointLight(PointLight &&light) noexcept
    : BaseLight{dynamic_cast<BaseLight &&>(light)},
      _linAttCoef{std::exchange(light._linAttCoef, 0.0f)},
      _quadAttCoef{std::exchange(light._quadAttCoef, 0.0f)} {}

// Move assignment operator
PointLight &PointLight::operator=(PointLight &&light) noexcept {
  this->BaseLight::operator=(dynamic_cast<BaseLight &&>(light));

  std::swap(_linAttCoef, light._linAttCoef);
  std::swap(_quadAttCoef, light._quadAttCoef);

  return *this;
}

// Destructor
PointLight::~PointLight() noexcept {}

// Setters

void PointLight::setLinAttCoef(float linAttCoef) noexcept { _linAttCoef = linAttCoef; }

void PointLight::setQuadAttCoef(float quadAttCoef) noexcept { _quadAttCoef = quadAttCoef; }

void PointLight::setShadowMapTextureResolution(GLsizei shadowMapTextureResolution) noexcept {
  _shadowMapTextureResolution = shadowMapTextureResolution;
}

// Getters

float PointLight::getLinAttCoef() const noexcept { return _linAttCoef; }

float &PointLight::getLinAttCoef() noexcept { return _linAttCoef; }

float PointLight::getQuadAttCoef() const noexcept { return _quadAttCoef; }

float &PointLight::getQuadAttCoef() noexcept { return _quadAttCoef; }
