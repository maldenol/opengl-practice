// Header file
#include "./mesh.hpp"

// Renders mesh
void glservice::renderMesh(const Mesh &mesh, const BaseCamera &camera) {
  // Setting specific shader program to use for render
  glUseProgram(mesh.shaderProgram);
  // Binding VAO with associated EBO and VBO
  glBindVertexArray(mesh.vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

  // For each texture
  for (size_t i = 0; i < mesh.textures.size(); ++i) {
    // Binding texture to texture unit
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, mesh.textures[i]);
  }

  // Updating shader uniform variables
  glUniformMatrix4fv(glGetUniformLocation(mesh.shaderProgram, "model"), 1, GL_FALSE,
                     glm::value_ptr(glm::mat4{1.0f}));
  glUniformMatrix4fv(glGetUniformLocation(mesh.shaderProgram, "view"), 1, GL_FALSE,
                     glm::value_ptr(camera.getViewMatrix()));
  glUniformMatrix4fv(glGetUniformLocation(mesh.shaderProgram, "proj"), 1, GL_FALSE,
                     glm::value_ptr(camera.getProjectionMatrix()));

  // Drawing mesh
  glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);

  // Unbinding configured VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Unbinding shader program
  glUseProgram(0);
}
