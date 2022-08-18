// Header file
#include "../Mesh.hpp"

// Calculates tangent by given positions and UVs of 3 points
glm::vec3 glengine::calculateTangent(const std::vector<glm::vec3> &pointPositions,
                                     const std::vector<glm::vec2> &pointUVs) {
  glm::vec3 deltaPos12 = pointPositions[1] - pointPositions[0];
  glm::vec3 deltaPos13 = pointPositions[2] - pointPositions[0];

  glm::vec2 deltaUV12 = pointUVs[1] - pointUVs[0];
  glm::vec2 deltaUV13 = pointUVs[2] - pointUVs[0];

  float f = 1.0f / (deltaUV12.x * deltaUV13.y - deltaUV13.x * deltaUV12.y);

  float tx = f * (deltaUV13.y * deltaPos12.x - deltaUV12.y * deltaPos13.x);
  float ty = f * (deltaUV13.y * deltaPos12.y - deltaUV12.y * deltaPos13.y);
  float tz = f * (deltaUV13.y * deltaPos12.z - deltaUV12.y * deltaPos13.z);

  return glm::normalize(glm::vec3{tx, ty, tz});
}
