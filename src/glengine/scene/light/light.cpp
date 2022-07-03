// Header file
#include "./light.hpp"

glengine::BaseLight::BaseLight(const glm::vec3 &color, float intensity) noexcept
    : color{color}, intensity{intensity} {}

glengine::BaseLight::~BaseLight() noexcept {}

glengine::DirectionalLight::DirectionalLight(const glm::vec3 &color, float intensity,
                                             const glm::vec3 &direction) noexcept
    : BaseLight{color, intensity}, direction{direction} {}

glengine::PointLight::PointLight(const glm::vec3 &color, float intensity, float linAttCoef,
                                 float quadAttCoef) noexcept
    : BaseLight{color, intensity}, linAttCoef{linAttCoef}, quadAttCoef{quadAttCoef} {}

glengine::SpotLight::SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction,
                               float linAttCoef, float quadAttCoef, float angle,
                               float smoothAngle) noexcept
    : BaseLight{color, intensity},
      direction{direction},
      linAttCoef{linAttCoef},
      quadAttCoef{quadAttCoef},
      angle{angle},
      smoothAngle{smoothAngle} {}
