// Header file
#include "./mesh.hpp"

// Generates mesh based on VBO attributes, vertex buffer, indices, textures and shader program
glservice::Mesh glservice::generateMesh(const std::vector<VBOAttribute> &vboAttributes,
                                        const std::vector<float>        &vertexBuffer,
                                        const std::vector<GLuint>       &indices,
                                        const std::vector<GLuint> &textures, GLuint shaderProgram) {
  GLuint vao = 0, vbo = 0;

  // Creating VAO, VBO and EBO
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  GLuint ebo{};
  glGenBuffers(1, &ebo);

  // Binding VAO to bind to it VBO and EBO and then configure them
  glBindVertexArray(vao);

  // Binding and filling VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0],
               GL_STATIC_DRAW);

  // Binding and filling EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0],
               GL_STATIC_DRAW);

  // Configuring and enabling VBO's attributes
  for (unsigned int i = 0; i < vboAttributes.size(); ++i) {
    glVertexAttribPointer(i, vboAttributes[i].size, vboAttributes[i].type,
                          vboAttributes[i].normalized, vboAttributes[i].stride,
                          vboAttributes[i].pointer);
    glEnableVertexAttribArray(i);
  }

  // Unbinding configured VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  return Mesh{vao, vbo, static_cast<GLsizei>(indices.size()), textures, shaderProgram};
}
