#ifndef GLENGINE_SCENE_SCENEOBJECT_HPP
#define GLENGINE_SCENE_SCENEOBJECT_HPP

// STD
#include <memory>
#include <vector>

// GLM
#include <glm/glm.hpp>

// "glengine" internal library
#include "./camera/camera.hpp"
#include "./light/light.hpp"
#include "./mesh/mesh.hpp"

namespace glengine {

// Scene object struct
struct SceneObject {
  // World orientation
  glm::vec3 translate{};
  glm::vec3 rotate{};
  glm::vec3 scale{1.0f};

  // Unity-like components
  std::shared_ptr<BaseLight> lightPtr{};
  std::shared_ptr<Mesh>      meshPtr{};
};

}  // namespace glengine

#endif
