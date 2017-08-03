#ifndef RUBUTILS_H
#define RUBUTILS_H
#include <glad/glad.h>
#include <cstdio>
#include <memory>
#include <ostream>
#include <sstream>
#include <iostream>

#define RETURN_IF_GL_ERROR(glexpr)                                             \
  do {                                                                         \
    (glexpr);                                                                  \
    GLenum error = glGetError();                                               \
    if (error != GL_NO_ERROR) {                                                \
      std::stringstream s;                                                     \
      s << "GL ERROR: 0x" << std::hex << error << " file:" << __FILE__         \
        << ", line: " << __LINE__;                                             \
      std::cout<<s.str()<<std::endl;\
throw s.str();								\
    }                                                                          \
  } while (false)

GLuint CompileShader(GLenum type, std::string text);
GLuint CreateShaderProgram(std::string vert_shader_fname,
                           std::string geom_shader_fname,
                           std::string frag_shader_fname);

#endif // RUBUTILS_H
