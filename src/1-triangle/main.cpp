// STD
#include <atomic>
#include <chrono>
#include <iostream>
#include <cmath>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Qt5
#include <QGuiApplication>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// STB
//#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// "glengine" internal library
#include <glengine.hpp>

using namespace glengine;

// for "ms"
using namespace std::chrono_literals;

// Global constants
static constexpr unsigned int          kWidth               = 800;
static constexpr unsigned int          kHeight              = 600;
static constexpr int                   kOpenGLVersionMajor  = 4;
static constexpr int                   kOpenGLVersionMinor  = 6;
static constexpr std::chrono::duration kRenderCycleInterval = 16ms;

// Functions for GLFW
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processUserInput(GLFWwindow *window);

// Functions for sceneObjects
void   initMesh(GLuint &vao, GLuint &vbo, GLuint &ebo, const std::vector<float> &vertices,
                const std::vector<GLuint> &indices);
GLuint initTexture(const std::string &filename);
void   drawMesh(GLuint vao, GLsizei indexCount, GLuint shaderProgram,
                const std::vector<GLuint> &textures);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication app = initQGuiApplication(argc, argv);

  // Initializing GLFW and getting configured window with OpenGL context
  initGLFW();
  GLFWwindow *window =
      createWindow(kWidth, kHeight, "1-triangle", kOpenGLVersionMajor, kOpenGLVersionMinor);

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting callback function on window resize
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Setting OpenGL clear color
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  // Enabling Z-testing
  glEnable(GL_DEPTH_TEST);

  // Creating arrays of filenames and types of shaders
  std::vector<GLenum> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  std::vector<std::string> shaderFilenames{
      getAbsolutePathRelativeToExecutable("triangleVS.glsl"),
      getAbsolutePathRelativeToExecutable("triangleFS.glsl"),
  };
  // Creating a shader program
  GLuint shaderProgram = glCreateProgram();
  // Running shaderWatcher thread
  std::mutex        glfwContextMutex{};
  std::atomic<bool> shaderWatcherIsRunning = true;
  std::atomic<bool> shadersAreRecompiled   = false;
  std::thread       shaderWatcherThread{shaderWatcher,
                                  std::cref(shaderWatcherIsRunning),
                                  std::ref(shadersAreRecompiled),
                                  window,
                                  std::ref(glfwContextMutex),
                                  shaderProgram,
                                  std::cref(shaderTypes),
                                  std::cref(shaderFilenames)};

  // Loading textures
  std::vector<GLuint> textures{
      initTexture("texture1.png"),
      initTexture("texture2.png"),
  };

  // Vertices and their indices to make a mesh from triangles
  std::vector<float> vertices{
      -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f,  // top-left
      0.5f,   0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  1.0f,  // top-right
      -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.25f, 0.0f,  // bottom-left
      0.25f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.75f, 0.0f,  // bottom-right
  };
  std::vector<GLuint> indices{
      0, 1, 2,  // top-left
      1, 3, 2,  // bottom-right
  };
  // Creating and configuring a mesh and getting its VAO, VBO and EBO
  GLuint vao{}, vbo{}, ebo{};
  initMesh(vao, vbo, ebo, vertices, indices);

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Render cycle
  while (true) {
    // Capturing mutex and OpenGL context
    std::unique_lock glfwContextLock{glfwContextMutex};
    glfwMakeContextCurrent(window);

    // If window should close
    if (glfwWindowShouldClose(window)) {
      // Releasing OpenGL context and mutex
      glfwMakeContextCurrent(nullptr);
      glfwContextLock.unlock();

      // Breaking render cycle
      break;
    }

    // Processing window events
    glfwPollEvents();

    // Processing user input
    processUserInput(window);

    // Clearing color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // If shaders are recompiled
    if (shadersAreRecompiled) {
      // Setting location of textures
      glUseProgram(shaderProgram);
      glUniform1i(glGetUniformLocation(shaderProgram, "texture0"), 0);
      glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 1);
      glUseProgram(0);

      // Notifying that all routine after shader recompilation is done
      shadersAreRecompiled = false;
    }

    // Drawing mesh
    drawMesh(vao, indices.size(), shaderProgram, textures);

    // Swapping front and back buffers
    glfwSwapBuffers(window);

    // Releasing OpenGL context and mutex
    glfwMakeContextCurrent(nullptr);
    glfwContextLock.unlock();

    std::this_thread::sleep_for(kRenderCycleInterval);
  }

  // Waiting for shaderWatcher to stop
  shaderWatcherIsRunning = false;
  shaderWatcherThread.join();

  // Deleting OpenGL objects
  for (size_t i = 0; i < textures.size(); ++i) {
    glDeleteTextures(1, &textures[i]);
  }
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glDeleteProgram(shaderProgram);

  // Terminating window with OpenGL context and GLFW
  terminateWindow(window);
  terminateGLFW();

  // Terminating Qt Gui application
  terminateQGuiApplication(app);

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Setting viewport position and size relative to window
  glViewport(0, 0, width, height);
}

void processUserInput(GLFWwindow *window) {
  static bool sPressed{};
  bool        released{true};

  // Terminating window
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    terminateWindow(window);
    return;
  }

  // Toggling fullscreen mode
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
    released = false;
    if (!sPressed) {
      sPressed = true;

      static int sPosX{}, sPosY{}, sWidth{}, sHeight{};
      if (glfwGetWindowMonitor(window) == nullptr) {
        enableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      } else {
        disableFullscreenMode(window, sPosX, sPosY, sWidth, sHeight);
      }
    }
  }

  // Setting polygon mode for both sides to GL_LINE
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  // Setting polygon mode for both sides to GL_FILL
  else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if (released) {
    sPressed = false;
  }
}

// Initializes mesh based on vertices and indices
void initMesh(GLuint &vao, GLuint &vbo, GLuint &ebo, const std::vector<float> &vertices,
              const std::vector<GLuint> &indices) {
  // Creating VAO, VBO and EBO
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  // Binding VAO to bind to it VBO and EBO and then configure them
  glBindVertexArray(vao);

  // Binding and filling VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

  // Binding and filling EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0],
               GL_STATIC_DRAW);

  // Configuring and enabling VBO's attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));

  // Unbinding configured VAO and VBO
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Initializes 2D texture
GLuint initTexture(const std::string &filename) {
  // Loading texture image
  stbi_set_flip_vertically_on_load(true);
  int            textureWidth{}, textureHeight{}, componentCount{};
  unsigned char *textureImage = stbi_load(getAbsolutePathRelativeToExecutable(filename).c_str(),
                                          &textureWidth, &textureHeight, &componentCount, 0);
  if (textureImage == nullptr) {
    std::cout << "error: failed to load image " << filename << std::endl;
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

// Draws mesh
void drawMesh(GLuint vao, GLsizei indexCount, GLuint shaderProgram,
              const std::vector<GLuint> &textures) {
  // Setting specific shader program to use for render
  glUseProgram(shaderProgram);
  // Binding VAO with associated VBO and EBO
  glBindVertexArray(vao);

  // For each texture
  for (size_t i = 0; i < textures.size(); ++i) {
    // Binding texture to texture unit
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures[i]);
  }

  // Updating shader uniform variables
  float mixRatio = std::cos(static_cast<float>(glfwGetTime())) / 2.0f + 0.5f;
  glUniform1f(glGetUniformLocation(shaderProgram, "mixRatio"), mixRatio);

  // Drawing mesh
  //glDrawArrays(GL_TRIANGLES, 0, 6); // without vertices (EBO) method
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
                 0);  // with indices (EBO) method

  // Unbinding VAO
  glBindVertexArray(0);
  // Unbinding shader program
  glUseProgram(0);
}
