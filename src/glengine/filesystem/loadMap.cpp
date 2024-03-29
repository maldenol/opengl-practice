// Header file
#include "./loadMap.hpp"

// STD
#include <iostream>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// "glengine" internal library
#include "./filesystem.hpp"

// Loads 2D texture
GLuint glengine::loadMap2D(const std::string &filename, bool sRGB) {
  // Creating texture
  GLuint texture{};
  glGenTextures(1, &texture);

  // Binding texture
  glBindTexture(GL_TEXTURE_2D, texture);

  stbi_set_flip_vertically_on_load(true);

  // Loading texture image
  int            textureWidth{}, textureHeight{}, componentCount{};
  unsigned char *textureImage =
      stbi_load(glengine::getAbsolutePathRelativeToExecutable(filename).c_str(), &textureWidth,
                &textureHeight, &componentCount, 0);
  if (textureImage == nullptr) {
    std::cout << "error: failed to load image " << filename << std::endl;
    // Freeing texture image memory
    stbi_image_free(textureImage);
    return 0;
  }

  // Filling texture with image data and generating mip-maps
  GLenum iformat = (sRGB ? GL_SRGB_ALPHA : GL_RGBA);
  GLenum oformat{};
  switch (componentCount) {
    case 1:
      oformat = GL_RED;
      break;
    case 2:
      oformat = GL_RG;
      break;
    case 3:
      oformat = GL_RGB;
      break;
    case 4:
      oformat = GL_RGBA;
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, iformat, textureWidth, textureHeight, 0, oformat, GL_UNSIGNED_BYTE,
               textureImage);
  // Freeing texture image memory
  stbi_image_free(textureImage);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Configuring texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbinding texture
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

// Loads HDR 2D texture
GLuint glengine::loadMap2DHDR(const std::string &filename) {
  // Creating texture
  GLuint texture{};
  glGenTextures(1, &texture);

  // Binding texture
  glBindTexture(GL_TEXTURE_2D, texture);

  stbi_set_flip_vertically_on_load(true);

  // Loading texture image
  int    textureWidth{}, textureHeight{}, componentCount{};
  float *textureImage = stbi_loadf(glengine::getAbsolutePathRelativeToExecutable(filename).c_str(),
                                   &textureWidth, &textureHeight, &componentCount, 0);
  if (textureImage == nullptr) {
    std::cout << "error: failed to load image " << filename << std::endl;
    // Freeing texture image memory
    stbi_image_free(textureImage);
    return 0;
  }

  // Filling texture with image data and generating mip-maps
  GLenum iformat = GL_RGBA16F;
  GLenum oformat{};
  switch (componentCount) {
    case 1:
      oformat = GL_RED;
      break;
    case 2:
      oformat = GL_RG;
      break;
    case 3:
      oformat = GL_RGB;
      break;
    case 4:
      oformat = GL_RGBA;
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, iformat, textureWidth, textureHeight, 0, oformat, GL_FLOAT,
               textureImage);
  // Freeing texture image memory
  stbi_image_free(textureImage);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Configuring texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbinding texture
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

// Loads cubemap
GLuint glengine::loadMapCube(const std::vector<std::string> &filenames, bool sRGB) {
  if (filenames.size() != 6) {
    return 0;
  }

  // Creating texture
  GLuint texture{};
  glGenTextures(1, &texture);

  // Binding texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

  stbi_set_flip_vertically_on_load(true);

  for (size_t i = 0; i < filenames.size(); ++i) {
    // Loading texture image
    int            textureWidth{}, textureHeight{}, componentCount{};
    unsigned char *textureImage =
        stbi_load(glengine::getAbsolutePathRelativeToExecutable(filenames[i]).c_str(),
                  &textureWidth, &textureHeight, &componentCount, 0);
    if (textureImage == nullptr) {
      std::cout << "error: failed to load image " << filenames[i] << std::endl;
      // Freeing texture image memory
      stbi_image_free(textureImage);
      return 0;
    }

    // Filling texture with image data
    GLenum iformat = (sRGB ? GL_SRGB_ALPHA : GL_RGBA);
    GLenum oformat{};
    switch (componentCount) {
      case 1:
        oformat = GL_RED;
        break;
      case 2:
        oformat = GL_RG;
        break;
      case 3:
        oformat = GL_RGB;
        break;
      case 4:
        oformat = GL_RGBA;
        break;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, iformat, textureWidth, textureHeight, 0,
                 oformat, GL_UNSIGNED_BYTE, textureImage);
    // Freeing texture image memory
    stbi_image_free(textureImage);
  }

  // Configuring texture
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbinding texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return texture;
}

// Loads HDR cubemap
GLuint glengine::loadMapCubeHDR(const std::vector<std::string> &filenames) {
  if (filenames.size() != 6) {
    return 0;
  }

  // Creating texture
  GLuint texture{};
  glGenTextures(1, &texture);

  // Binding texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

  stbi_set_flip_vertically_on_load(true);

  for (size_t i = 0; i < filenames.size(); ++i) {
    // Loading texture image
    int    textureWidth{}, textureHeight{}, componentCount{};
    float *textureImage =
        stbi_loadf(glengine::getAbsolutePathRelativeToExecutable(filenames[i]).c_str(),
                   &textureWidth, &textureHeight, &componentCount, 0);
    if (textureImage == nullptr) {
      std::cout << "error: failed to load image " << filenames[i] << std::endl;
      // Freeing texture image memory
      stbi_image_free(textureImage);
      return 0;
    }

    // Filling texture with image data
    GLenum iformat = GL_RGBA16F;
    GLenum oformat{};
    switch (componentCount) {
      case 1:
        oformat = GL_RED;
        break;
      case 2:
        oformat = GL_RG;
        break;
      case 3:
        oformat = GL_RGB;
        break;
      case 4:
        oformat = GL_RGBA;
        break;
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, iformat, textureWidth, textureHeight, 0,
                 oformat, GL_FLOAT, textureImage);
    // Freeing texture image memory
    stbi_image_free(textureImage);
  }

  // Configuring texture
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbinding texture
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  return texture;
}
