// STD
#include <atomic>
#include <chrono>
#include <iostream>
#include <cmath>
#include <mutex>
#include <thread>
#include <vector>

// Qt5
#include <QGuiApplication>
#include <QString>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Internal library "glengine"
#include <glengine/glengine.hpp>

// for "ms"
using namespace std::chrono_literals;

// Global constants
constexpr unsigned int          kWidth               = 800;
constexpr unsigned int          kHeight              = 600;
constexpr std::chrono::duration kRenderCycleInterval = 16ms;

// Functions for GLFW
void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processUserInput(GLFWwindow *window);

// Functions for meshes
GLuint initMesh(const std::vector<float>  &vertices,
                const std::vector<GLuint> &indices);
GLuint initTexture(const QString &filename);
void   drawMesh(GLuint vao, GLsizei indexCount, GLuint shaderProgram,
                const std::vector<GLuint> &textures);

// Main function
int main(int argc, char *argv[]) {
  // Initializing Qt Gui application
  QGuiApplication app = glengine::initQGuiApplication(argc, argv);

  // Initializing GLFW
  glengine::initGLFW();

  // Creating GLFW window and loading OpenGL functions with GLAD
  GLFWwindow *window = glengine::initWindow(kWidth, kHeight, "triangle");

  // Capturing OpenGL context
  glfwMakeContextCurrent(window);

  // Setting callback function on window resize
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Creating arrays of shaders, according filenames and types
  std::vector<GLuint>  shaders{0, 0};
  std::vector<QString> shaderFileNames{
      glengine::getAbsolutePathRelativeToExecutable("vertexShader.glsl"),
      glengine::getAbsolutePathRelativeToExecutable("fragmentShader.glsl"),
  };
  std::vector<GLuint> shaderTypes{
      GL_VERTEX_SHADER,
      GL_FRAGMENT_SHADER,
  };
  // Creating and configuring a shader program
  GLuint shaderProgram = glengine::initShaderProgram(shaders, shaderTypes);
  // Running shaderWatcher thread
  std::mutex        glfwContextMutex{};
  std::atomic<bool> shaderWatcherIsRunning = true;
  std::thread       shaderWatcherThread{glengine::shaderWatcher,
                                  std::cref(shaderWatcherIsRunning),
                                  window,
                                  std::ref(glfwContextMutex),
                                  shaderProgram,
                                  std::cref(shaders),
                                  std::cref(shaderFileNames)};

  // Loading textures
  std::vector<GLuint> textures{
      initTexture("texture1.png"),
      initTexture("texture2.png"),
  };
  // Setting location of textures
  glUseProgram(shaderProgram);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);
  glUseProgram(0);

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
  // Creating and configuring a mesh and getting its VAO
  GLuint vao = initMesh(vertices, indices);

  // Releasing OpenGL context
  glfwMakeContextCurrent(nullptr);

  // Render cycle
  while (true) {
    // Capturing mutex and OpenGL context
    std::unique_lock glfwContextLock{glfwContextMutex};
    glfwMakeContextCurrent(window);

    // If window should close
    if (glfwWindowShouldClose(window)) break;

    // Processing window events
    glfwPollEvents();

    // Processing user input
    processUserInput(window);

    // Clearing background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

  // Terminating GLFW
  glengine::terminateGLFW();

  // Terminating Qt Gui application
  glengine::terminateQGuiApplication(app);

  return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  // Setting viewport position and size relative to window
  glViewport(0, 0, width, height);
}

void processUserInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    // Terminating window
    glengine::terminateWindow(window);
    return;
  }

  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    // Setting polygon mode for both sides to GL_LINE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    // Setting polygon mode for both sides to GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

// Initializes mesh based on vertices and indices
GLuint initMesh(const std::vector<float>  &vertices,
                const std::vector<GLuint> &indices) {
  // Creating VAO, VBO and EBO
  GLuint vao{};
  glGenVertexArrays(1, &vao);
  GLuint vbo{};
  glGenBuffers(1, &vbo);
  GLuint ebo{};
  glGenBuffers(1, &ebo);

  // Binding VAO to bind to it VBO and EBO and then configure them
  glBindVertexArray(vao);

  // Binding and filling VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0],
               GL_STATIC_DRAW);

  // Binding and filling EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
               &indices[0], GL_STATIC_DRAW);

  // Configuring and enabling VBO's attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void *>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Unbinding configured VAO to use it later
  glBindVertexArray(0);

  return vao;
}

// Initializes 2D texture
GLuint initTexture(const QString &filename) {
  // Loading texture image
  stbi_set_flip_vertically_on_load(true);
  int            textureWidth{}, textureHeight{}, componentCount{};
  unsigned char *textureImage =
      stbi_load(glengine::getAbsolutePathRelativeToExecutable(filename)
                    .toLocal8Bit()
                    .data(),
                &textureWidth, &textureHeight, &componentCount, 0);
  if (textureImage == nullptr) {
    std::cout << "error: failed to load image " << filename.toStdString()
              << std::endl;
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, format,
               GL_UNSIGNED_BYTE, textureImage);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Configuring texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
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
  float mixRatio = cos(static_cast<float>(glfwGetTime())) / 2.0f + 0.5f;
  glUniform1f(glGetUniformLocation(shaderProgram, "mixRatio"), mixRatio);

  // Drawing mesh
  //glDrawArrays(GL_TRIANGLES, 0, 3); // without vertices (EBO) method
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
                 0);  // with vertices (EBO) method

  // Unbinding VAO
  glBindVertexArray(0);
  // Unbinding shader program
  glUseProgram(0);
}