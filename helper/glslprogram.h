#pragma once

#ifdef WIN32
#pragma warning(disable : 4290)
#endif

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <string>

#include "glutils.h"

class GLSLProgramException : public std::runtime_error {
public:
  GLSLProgramException(const std::string &msg) : std::runtime_error(msg) {}
};

namespace GLSLShader {
enum GLSLShaderType {
  VERTEX = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER,
  TESS_CONTROL = GL_TESS_CONTROL_SHADER,
  TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
  COMPUTE = GL_COMPUTE_SHADER
};
};

class GLSLProgram {
private:
  GLuint handle;
  bool linked;
  std::map<std::string, int> uniformLocations;

  inline GLint getUniformLocation(const char *name);
  void detachAndDeleteShaderObjects();
  bool fileExists(const std::string &fileName);
  std::string getExtension(const char *fileName);

public:
  GLSLProgram();
  ~GLSLProgram();

  // Make it non-copyable.
  GLSLProgram(const GLSLProgram &) = delete;
  GLSLProgram &operator=(const GLSLProgram &) = delete;

  void compileShader(const char *fileName);
  void compileShader(const char *fileName, GLSLShader::GLSLShaderType type);
  void compileShader(const std::string &source, GLSLShader::GLSLShaderType type,
                     const char *fileName = NULL);

  void link();
  void validate();
  void use();

  int getHandle();
  bool isLinked();

  void bindAttribLocation(GLuint location, const char *name);
  void bindFragDataLocation(GLuint location, const char *name);

  void setUniform(const char *name, float x, float y, float z);
  void setUniform(const char *name, const glm::vec2 &v);
  void setUniform(const char *name, const glm::vec3 &v);
  void setUniform(const char *name, const glm::vec4 &v);
  void setUniform(const char *name, const glm::mat4 &m);
  void setUniform(const char *name, const glm::mat3 &m);
  void setUniform(const char *name, float val);
  void setUniform(const char *name, int val);
  void setUniform(const char *name, bool val);
  void setUniform(const char *name, GLuint val);

  void findUniformLocations();
  void printActiveUniforms();
  void printActiveUniformBlocks();
  void printActiveAttribs();

  const char *getTypeString(GLenum type);
};

int GLSLProgram::getUniformLocation(const char *name) {
#ifndef NDEBUG
  int currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  assert(currentProgram == handle);

  GLint loc = glGetUniformLocation(handle, name);
  assert(loc >= 0 && "glGetUniformLocation fail");
  GLUtils::checkForOpenGLError(__FILE__, __LINE__);

  uniformLocations[name] = loc;
  return loc;
#else
  auto pos = uniformLocations.find(name);

  if (pos == uniformLocations.end()) {
    GLint loc = glGetUniformLocation(handle, name);
    uniformLocations[name] = loc;
    return loc;
  }

  return pos->second;

#endif
}
