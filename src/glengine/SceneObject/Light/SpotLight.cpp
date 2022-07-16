// Header file
#include "./SpotLight.hpp"

// STD
#include <utility>

using namespace glengine;

// Local function headers

static void configureShadowMapFramebuffer(GLuint shadowMapFBO, GLuint shadowMapTexture,
                                          GLsizei shadowMapTextureResolution);
static void copyShadowMapFramebuffer(GLuint srcShadowMapFBO, GLsizei srcShadowMapTextureResolution,
                                     GLuint dstShadowMapFBO, GLsizei dstShadowMapTextureResolution);

// Constructors, assignment operators and destructor

// Default constructor
SpotLight::SpotLight() noexcept {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Parameterized constructor
SpotLight::SpotLight(const glm::vec3 &color, float intensity, const glm::vec3 &direction,
                     float linAttCoef, float quadAttCoef, float angle, float smoothAngle) noexcept
    : BaseLight{color, intensity},
      DirectionalLight{color, intensity, direction},
      PointLight{color, intensity, linAttCoef, quadAttCoef},
      _angle{angle},
      _smoothAngle{smoothAngle} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy constructor (base class)
SpotLight::SpotLight(const BaseLight &light) noexcept : BaseLight{light} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy assignment operator (base class)
SpotLight &SpotLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);

  return *this;
}

// Copy constructor (base class)
SpotLight::SpotLight(const DirectionalLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy assignment operator (base class)
SpotLight &SpotLight::operator=(const DirectionalLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);

  return *this;
}

// Copy constructor (base class)
SpotLight::SpotLight(const PointLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy assignment operator (base class)
SpotLight &SpotLight::operator=(const PointLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);

  return *this;
}

// Copy constructor
SpotLight::SpotLight(const SpotLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)},
      DirectionalLight{dynamic_cast<const DirectionalLight &>(light)},
      PointLight{dynamic_cast<const PointLight &>(light)},
      _angle{light._angle},
      _smoothAngle{light._smoothAngle} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
  copyShadowMapFramebuffer(light._shadowMapFBO, light._shadowMapTextureResolution, _shadowMapFBO,
                           _shadowMapTextureResolution);
}

// Copy assignment operator
SpotLight &SpotLight::operator=(const SpotLight &light) noexcept {
  this->BaseLight::       operator=(dynamic_cast<const BaseLight &>(light));
  this->DirectionalLight::operator=(dynamic_cast<const DirectionalLight &>(light));
  this->PointLight::      operator=(dynamic_cast<const PointLight &>(light));

  _angle       = light._angle;
  _smoothAngle = light._smoothAngle;

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
  copyShadowMapFramebuffer(light._shadowMapFBO, light._shadowMapTextureResolution, _shadowMapFBO,
                           _shadowMapTextureResolution);

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

void SpotLight::setShadowMapTextureResolution(GLsizei shadowMapTextureResolution) noexcept {
  _shadowMapTextureResolution = shadowMapTextureResolution;

  // Deleting shadow map framebuffer and creating a new one
  glDeleteTextures(1, &_shadowMapTexture);
  glDeleteFramebuffers(1, &_shadowMapFBO);
  glGenFramebuffers(1, &_shadowMapFBO);
  glGenTextures(1, &_shadowMapTexture);
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Getters

float SpotLight::getAngle() const noexcept { return _angle; }

float &SpotLight::getAngle() noexcept { return _angle; }

float SpotLight::getSmoothAngle() const noexcept { return _smoothAngle; }

float &SpotLight::getSmoothAngle() noexcept { return _smoothAngle; }

// Local function definition

static void configureShadowMapFramebuffer(GLuint shadowMapFBO, GLuint shadowMapTexture,
                                          GLsizei shadowMapTextureResolution) {
  // Creating and configuring shadow map depth texture
  glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapTextureResolution,
               shadowMapTextureResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Creating and configuring shadow map FBO
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

static void copyShadowMapFramebuffer(GLuint srcShadowMapFBO, GLsizei srcShadowMapTextureResolution,
                                     GLuint  dstShadowMapFBO,
                                     GLsizei dstShadowMapTextureResolution) {
  // Copying shadow map framebuffer
  glBindFramebuffer(GL_READ_FRAMEBUFFER, srcShadowMapFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstShadowMapFBO);
  glBlitFramebuffer(0, 0, srcShadowMapTextureResolution, srcShadowMapTextureResolution, 0, 0,
                    dstShadowMapTextureResolution, dstShadowMapTextureResolution,
                    GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
