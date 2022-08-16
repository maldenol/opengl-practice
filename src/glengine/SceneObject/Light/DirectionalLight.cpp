// Header file
#include "./DirectionalLight.hpp"

// STD
#include <utility>

// OpenGL
#include <glad/glad.h>

using namespace glengine;

// Local function headers

static void configureShadowMapFramebuffer(GLuint shadowMapFBO, GLuint shadowMapTexture,
                                          GLsizei shadowMapTextureResolution);
static void copyShadowMapFramebuffer(GLuint srcShadowMapFBO, GLsizei srcShadowMapTextureResolution,
                                     GLuint dstShadowMapFBO, GLsizei dstShadowMapTextureResolution);

// Constructors, assignment operators and destructor

// Default constructor
DirectionalLight::DirectionalLight() noexcept {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Parameterized constructor
DirectionalLight::DirectionalLight(const glm::vec3 &color, const glm::vec3 &direction) noexcept
    : BaseLight{color}, _direction{direction} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy constructor (base class)
DirectionalLight::DirectionalLight(const BaseLight &light) noexcept : BaseLight{light} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Copy assignment operator (base class)
DirectionalLight &DirectionalLight::operator=(const BaseLight &light) noexcept {
  this->BaseLight::operator=(light);

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);

  return *this;
}

// Copy constructor
DirectionalLight::DirectionalLight(const DirectionalLight &light) noexcept
    : BaseLight{dynamic_cast<const BaseLight &>(light)}, _direction{light._direction} {
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
  copyShadowMapFramebuffer(light._shadowMapFBO, light._shadowMapTextureResolution, _shadowMapFBO,
                           _shadowMapTextureResolution);
}

// Copy assignment operator
DirectionalLight &DirectionalLight::operator=(const DirectionalLight &light) noexcept {
  this->BaseLight::operator=(dynamic_cast<const BaseLight &>(light));

  _direction = light._direction;

  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
  copyShadowMapFramebuffer(light._shadowMapFBO, light._shadowMapTextureResolution, _shadowMapFBO,
                           _shadowMapTextureResolution);

  return *this;
}

// Move constructor
DirectionalLight::DirectionalLight(DirectionalLight &&light) noexcept
    : BaseLight{dynamic_cast<BaseLight &&>(light)},
      _direction{std::exchange(light._direction, glm::vec3{})} {}

// Move assignment operator
DirectionalLight &DirectionalLight::operator=(DirectionalLight &&light) noexcept {
  this->BaseLight::operator=(dynamic_cast<BaseLight &&>(light));

  std::swap(_direction, light._direction);

  return *this;
}

// Destructor
DirectionalLight::~DirectionalLight() noexcept {}

// Setters

void DirectionalLight::setDirection(const glm::vec3 &direction) noexcept { _direction = direction; }

void DirectionalLight::setShadowMapTextureResolution(GLsizei shadowMapTextureResolution) noexcept {
  _shadowMapTextureResolution = shadowMapTextureResolution;

  // Deleting shadow map framebuffer and creating a new one
  glDeleteTextures(1, &_shadowMapTexture);
  glDeleteFramebuffers(1, &_shadowMapFBO);
  glGenFramebuffers(1, &_shadowMapFBO);
  glGenTextures(1, &_shadowMapTexture);
  configureShadowMapFramebuffer(_shadowMapFBO, _shadowMapTexture, _shadowMapTextureResolution);
}

// Getters

const glm::vec3 &DirectionalLight::getDirection() const noexcept { return _direction; }

glm::vec3 &DirectionalLight::getDirection() noexcept { return _direction; }

// Local function definition

static void configureShadowMapFramebuffer(GLuint shadowMapFBO, GLuint shadowMapTexture,
                                          GLsizei shadowMapTextureResolution) {
  // Creating and configuring shadow map depth texture
  glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapTextureResolution,
               shadowMapTextureResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
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
