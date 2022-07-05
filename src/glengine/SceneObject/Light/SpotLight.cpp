// Header file
#include "./SpotLight.hpp"

// STD
#include <utility>

using namespace glengine;

// Constructors, assignment operators and destructor

// Default constructor
SpotLight::SpotLight() noexcept {}

// Parameterized constructor
SpotLight::SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction,
                     float linAttCoef, float quadAttCoef, float angle, float smoothAngle) noexcept
    : BaseLight{color, intensity},
      DirectionalLight{color, intensity, direction},
      PointLight{color, intensity, linAttCoef, quadAttCoef},
      _angle{angle},
      _smoothAngle{smoothAngle} {}

// Copy constructor (base class)
SpotLight::SpotLight(const BaseLight &light) noexcept : BaseLight{light} {}

// Copy assignment operator (base class)
SpotLight &SpotLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  return *this;
}

// Copy constructor (base class)
SpotLight::SpotLight(const DirectionalLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)} {}

// Copy assignment operator (base class)
SpotLight &SpotLight::operator=(const DirectionalLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  return *this;
}

// Copy constructor (base class)
SpotLight::SpotLight(const PointLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)} {}

// Copy assignment operator (base class)
SpotLight &SpotLight::operator=(const PointLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  return *this;
}

// Copy constructor
SpotLight::SpotLight(const SpotLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)},
      DirectionalLight{dynamic_cast<const DirectionalLight &>(light)},
      PointLight{dynamic_cast<const PointLight &>(light)},
      _angle{light._angle},
      _smoothAngle{light._smoothAngle} {}

// Copy assignment operator
SpotLight &SpotLight::operator=(const SpotLight &light) noexcept {
  this->BaseLight::       operator=(dynamic_cast<const BaseLight &>(light));
  this->DirectionalLight::operator=(dynamic_cast<const DirectionalLight &>(light));
  this->PointLight::      operator=(dynamic_cast<const PointLight &>(light));

  _angle       = light._angle;
  _smoothAngle = light._smoothAngle;

  return *this;
}

// Move constructor
SpotLight::SpotLight(SpotLight &&light) noexcept
    : BaseLight{dynamic_cast<BaseLight &&>(light)},
      DirectionalLight{dynamic_cast<DirectionalLight &&>(light)},
      PointLight{dynamic_cast<PointLight &&>(light)},
      _angle{std::exchange(light._angle, 0.0f)},
      _smoothAngle{std::exchange(light._smoothAngle, 0.0f)} {}

// Move assignment operator
SpotLight &SpotLight::operator=(SpotLight &&light) noexcept {
  this->BaseLight::       operator=(dynamic_cast<BaseLight &&>(light));
  this->DirectionalLight::operator=(dynamic_cast<DirectionalLight &&>(light));
  this->PointLight::      operator=(dynamic_cast<PointLight &&>(light));

  std::swap(_angle, light._angle);
  std::swap(_smoothAngle, light._smoothAngle);

  return *this;
}

// Destructor
SpotLight::~SpotLight() noexcept {}

// Setters

void SpotLight::setAngle(float angle) noexcept { _angle = angle; }

void SpotLight::setSmoothAngle(float smoothAngle) noexcept { _smoothAngle = smoothAngle; }

// Getters

float SpotLight::getAngle() const noexcept { return _angle; }

float &SpotLight::getAngle() noexcept { return _angle; }

float SpotLight::getSmoothAngle() const noexcept { return _smoothAngle; }

float &SpotLight::getSmoothAngle() noexcept { return _smoothAngle; }
