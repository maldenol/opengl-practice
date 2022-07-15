#ifndef GLENGINE_SCENEOBJECT_LIGHT_BASELIGHT_HPP
#define GLENGINE_SCENEOBJECT_LIGHT_BASELIGHT_HPP

// OpenGL
#include <glad/glad.h>

// GLM
#include <glm/glm.hpp>

namespace glengine {

constexpr GLsizei kInitShadowMapTextureResolution = 1024;

// Base light class
class BaseLight {
 private:
  glm::vec3 _color{};
  float     _intensity{};

 protected:
  GLuint  _shadowMapFBO{};
  GLuint  _shadowMapTexture{};
  GLsizei _shadowMapTextureResolution{kInitShadowMapTextureResolution};

 protected:
  // Constructors, assignment operators and destructor
  BaseLight() noexcept;
  BaseLight(const glm::vec3 &color, float intensity) noexcept;
  BaseLight(const BaseLight &light) noexcept;
  BaseLight &operator=(const BaseLight &light) noexcept;
  BaseLight(BaseLight &&light) noexcept;
  BaseLight &operator=(BaseLight &&light) noexcept;
  virtual ~BaseLight() noexcept;

 public:
  // Setters
  void         setColor(const glm::vec3 &color) noexcept;
  void         setIntensity(float intensity) noexcept;
  void         setShadowMapFBO(GLuint shadowMapFBO) noexcept;
  void         setShadowMapTexture(GLuint shadowMapTexture) noexcept;
  virtual void setShadowMapTextureResolution(GLsizei shadowMapTextureResolution) noexcept = 0;

  // Getters
  const glm::vec3 &getColor() const noexcept;
  glm::vec3       &getColor() noexcept;
  float            getIntensity() const noexcept;
  float           &getIntensity() noexcept;
  GLuint           getShadowMapFBO() const noexcept;
  GLuint          &getShadowMapFBO() noexcept;
  GLuint           getShadowMapTexture() const noexcept;
  GLuint          &getShadowMapTexture() noexcept;
  GLsizei          getShadowMapTextureResolution() const noexcept;
  GLsizei         &getShadowMapTextureResolution() noexcept;
};

}  // namespace glengine

#endif
