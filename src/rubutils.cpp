#include "rubutils.h"
#include <vector>
#include <sstream>
// clang-format off

// clang-format on

// Compiles a shader with the given type and returns the handle.
GLuint CompileShader(GLenum type, std::string text) {

  auto shader_id = glCreateShader(type);
  if (shader_id != 0) {
    const auto* text_str = text.data();
    RETURN_IF_GL_ERROR(glShaderSource(shader_id, 1, &text_str, NULL));
    RETURN_IF_GL_ERROR(glCompileShader(shader_id));
    GLint shader_ok;
    RETURN_IF_GL_ERROR(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_ok));
    if (shader_ok != GL_TRUE) {
      GLsizei log_length;
      RETURN_IF_GL_ERROR(
          glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length));

      std::vector<char> info_log(log_length + 1);
      RETURN_IF_GL_ERROR(
          glGetShaderInfoLog(shader_id, log_length, nullptr, &info_log[0]));
      RETURN_IF_GL_ERROR(glDeleteShader(shader_id));
      std::stringstream s;
      s<<"Failed to compile shader with error: "<< &info_log[0];
      std::cout<< s.str()<<std::endl;
      throw s.str();
    }
  }
  return shader_id;
}

// Compiles and links a shader program, using the provided vertex and fragment
// shaders.
GLuint CreateShaderProgram(std::string vert_shader_data,
			   std::string geom_shader_data,
			   std::string frag_shader_data) {
  printf("compiling vertex shader ....\n");
  // compile vertex shader
  auto vertex_shader= CompileShader(GL_VERTEX_SHADER, vert_shader_data);
  printf("compiling vertex shader done id : %d \n",vertex_shader);
  // compile vertex shader
  auto geometry_shader= CompileShader(GL_GEOMETRY_SHADER, geom_shader_data);
  printf("compiling geometry shader done id : %d \n",geometry_shader);
  // compile fragment shader
  auto fragment_shader= CompileShader(GL_FRAGMENT_SHADER, frag_shader_data);
  printf("compiling fragment shader done id : %d \n",fragment_shader);

  // link program
  GLuint program_id;
  RETURN_IF_GL_ERROR(program_id = glCreateProgram());
  printf("program_id : %d \n",program_id);
  RETURN_IF_GL_ERROR(glAttachShader(program_id, vertex_shader));
  printf("attached vertex shader \n");
  RETURN_IF_GL_ERROR(glAttachShader(program_id, geometry_shader));
  printf("attached geometry shader \n");
  RETURN_IF_GL_ERROR(glAttachShader(program_id, fragment_shader));
  printf("attached fragment shader \n");
  RETURN_IF_GL_ERROR(glLinkProgram(program_id));
  printf("linked program \n");
  RETURN_IF_GL_ERROR(glDeleteShader(fragment_shader));
  RETURN_IF_GL_ERROR(glDeleteShader(geometry_shader));
  RETURN_IF_GL_ERROR(glDeleteShader(vertex_shader));

  GLint program_ok;
  RETURN_IF_GL_ERROR(glGetProgramiv(program_id, GL_LINK_STATUS, &program_ok));

  if (program_ok != GL_TRUE) {
    printf("program_ok : %d\n",program_ok);
    GLsizei log_length=0;
    RETURN_IF_GL_ERROR(
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length));
    printf("log_length : %d\n",log_length);
    std::vector<char> info_log(log_length + 1);
    RETURN_IF_GL_ERROR(
        glGetProgramInfoLog(program_id, log_length, nullptr, &info_log[0]));
    RETURN_IF_GL_ERROR(glDeleteProgram(program_id));
    std::stringstream s;
    s<< "Failed to compile shader program_id with error: "<< &info_log[0];
    std::cout<<s.str()<<std::endl;
    throw s.str();
  } else {
    printf("your gl program seemed to compile fine\n");
  }
  return program_id;
}

