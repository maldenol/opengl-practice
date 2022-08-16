// Header file
#include "./BaseLight.hpp"

// STD
#include <utility>

// OpenGL
#include <glad/glad.h>

using namespace glengine;

// Constructors, assignment operators and destructor

// Local function headers

void createShadowMapFramebuffer(GLuint &shadowMapFBO, GLuint &shadowMapTexture);

// Default constructor
BaseLight::BaseLight() noexcept { createShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture); }

// Parameterized constructor
BaseLight::BaseLight(const glm::vec3 &color) noexcept : _color{color} {
  createShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture);
}

// Copy constructor
BaseLight::BaseLight(const BaseLight &light) noexcept : _color{light._color} {
  createShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture);
}

// Copy assignment operator
BaseLight &BaseLight::operator=(const BaseLight &light) noexcept {
  _color = light._color;

  createShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture);

  return *this;
}

// Move constructor
BaseLight::BaseLight(BaseLight &&light) noexcept
    : _color{std::exchange(light._color, glm::vec3{})},
      _shadowMapFBO{std::exchange(light._shadowMapFBO, 0)},
      _shadowMapTexture{std::exchange(light._shadowMapTexture, 0)} {}

// Move assignment operator
BaseLight &BaseLight::operator=(BaseLight &&light) noexcept {
  std::swap(_color, light._color);
  std::swap(_shadowMapFBO, light._shadowMapFBO);
  std::swap(_shadowMapTexture, light._shadowMapTexture);

  return *this;
}

// Destructor
BaseLight::~BaseLight() noexcept {
  glDeleteTextures(1, &_shadowMapTexture);
  glDeleteFramebuffers(1, &_shadowMapFBO);
}

// Setters

void BaseLight::setColor(const glm::vec3 &color) noexcept { _color = color; }

void BaseLight::setShadowMapFBO(GLuint shadowMapFBO) noexcept { _shadowMapFBO = shadowMapFBO; }

void BaseLight::setShadowMapTexture(GLuint shadowMapTexture) noexcept {
  _shadowMapTexture = shadowMapTexture;
}

// Getters

const glm::vec3 &BaseLight::getColor() const noexcept { return _color; }

glm::vec3 &BaseLight::getColor() noexcept { return _color; }

GLuint BaseLight::getShadowMapFBO() const noexcept { return _shadowMapFBO; }

GLuint &BaseLight::getShadowMapFBO() noexcept { return _shadowMapFBO; }

GLuint BaseLight::getShadowMapTexture() const noexcept { return _shadowMapTexture; }

GLuint &BaseLight::getShadowMapTexture() noexcept { return _shadowMapTexture; }

GLsizei BaseLight::getShadowMapTextureResolution() const noexcept {
  return _shadowMapTextureResolution;
}

GLsizei &BaseLight::getShadowMapTextureResolution() noexcept { return _shadowMapTextureResolution; }

// Local function definition

void createShadowMapFramebuffer(GLuint &shadowMapFBO, GLuint &shadowMapTexture) {
  // Creating shadow map FBO and depth texture
  glGenFramebuffers(1, &shadowMapFBO);
  glGenTextures(1, &shadowMapTexture);
}
