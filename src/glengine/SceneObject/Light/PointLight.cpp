// Header file
#include "./PointLight.hpp"

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
PointLight::PointLight() noexcept : BaseLight{} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Parameterized constructor
PointLight::PointLight(const glm::vec3 &color, float linAttCoef, float quadAttCoef) noexcept
    : BaseLight{color}, _linAttCoef{linAttCoef}, _quadAttCoef{quadAttCoef} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy constructor (base class)
PointLight::PointLight(const BaseLight &light) noexcept : BaseLight{light} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy assignment operator (base class)
PointLight &PointLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);

  return *this;
}

// Copy constructor
PointLight::PointLight(const PointLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)},
      _linAttCoef{light._linAttCoef},
      _quadAttCoef{light._quadAttCoef} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
  copyShadowMapFramebuffer(light._shadowMapFBO, light._shadowMapTextureResolution, _shadowMapFBO,
                           _shadowMapTextureResolution);
}

// Copy assignment operator
PointLight &PointLight::operator=(const PointLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  _linAttCoef  = light._linAttCoef;
  _quadAttCoef = light._quadAttCoef;

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
  copyShadowMapFramebuffer(light._shadowMapFBO, light._shadowMapTextureResolution, _shadowMapFBO,
                           _shadowMapTextureResolution);

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

  // Deleting shadow map framebuffer and creating a new one
  glDeleteTextures(1, &_shadowMapTexture);
  glDeleteFramebuffers(1, &_shadowMapFBO);
  glGenFramebuffers(1, &_shadowMapFBO);
  glGenTextures(1, &_shadowMapTexture);
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Getters

float PointLight::getLinAttCoef() const noexcept { return _linAttCoef; }

float &PointLight::getLinAttCoef() noexcept { return _linAttCoef; }

float PointLight::getQuadAttCoef() const noexcept { return _quadAttCoef; }

float &PointLight::getQuadAttCoef() noexcept { return _quadAttCoef; }

// Local function definition

static void configureShadowMapFramebuffer(GLuint shadowMapFBO, GLuint shadowMapTexture,
                                          GLsizei shadowMapTextureResolution) {
  // Creating and configuring shadow map depth texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMapTexture);
  for (unsigned int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 shadowMapTextureResolution, shadowMapTextureResolution, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Creating and configuring shadow map FBO
  glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMapTexture, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
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
