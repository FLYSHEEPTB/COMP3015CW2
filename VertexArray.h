#pragma once

// glad must be the first one
#include <glad/glad.h>

#include <vector>

struct VertexArray {
  GLuint gVBO = 0;  // vertex buffer object identifier
  GLuint gVAO = 0;  // vertex array object identifier

  VertexArray(const std::vector<GLfloat> &vertices);
};
