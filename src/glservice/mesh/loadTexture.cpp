// Header file
#include "./mesh.hpp"

// STD
#include <iostream>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// "glservice" internal library
#include "../filesystem.hpp"

// Loads 2D texture
GLuint glservice::loadTexture(const QString &filename) {
  // Loading texture image
  stbi_set_flip_vertically_on_load(true);
  int            textureWidth{}, textureHeight{}, componentCount{};
  unsigned char *textureImage =
      stbi_load(glservice::getAbsolutePathRelativeToExecutable(filename).toLocal8Bit().data(),
                &textureWidth, &textureHeight, &componentCount, 0);
  if (textureImage == nullptr) {
    std::cout << "error: failed to load image " << filename.toStdString() << std::endl;
    // Freeing texture image memory
    stbi_image_free(textureImage);
    return 0;
  }

  // Creating texture
  GLuint texture{};
  glGenTextures(1, &texture);

  // Binding texture
  glBindTexture(GL_TEXTURE_2D, texture);

  // Filling texture with image data and generating mip-maps
  GLenum format = (componentCount == 3) ? GL_RGB : GL_RGBA;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, format, GL_UNSIGNED_BYTE,
               textureImage);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Configuring texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbinding texture
  glBindTexture(GL_TEXTURE_2D, 0);

  // Freeing texture image memory
  stbi_image_free(textureImage);

  return texture;
}