#include "ncube_renderer.h"
#include <fstream>
#include <iostream>
#include <sstream>
void setUniformData(GLuint program_id, const char *name, int size,
                    const int *data) {
  GLuint loc;
  RETURN_IF_GL_ERROR(loc = glGetUniformLocation(program_id, name));
  RETURN_IF_GL_ERROR(glProgramUniform1iv(program_id, loc, size, data));
  std::cout << "uniform : " << name << " location : " << loc << std::endl;
  for (int i = 0; i < size; i++) {
    int retdata;
    std::stringstream s1;
    s1 << name << "[" << i << "]";

    int iLoc = glGetUniformLocation(program_id, s1.str().c_str());
    RETURN_IF_GL_ERROR(
        glGetnUniformiv(program_id, iLoc, sizeof(int), &(retdata)));
    if (retdata != data[i]) {
      std::stringstream s;
      s << "name : " << name << " loc : " << loc << " size : " << size
        << " index : " << i << " expected : " << data[i]
        << " actual : " << retdata;
      std::cout << s.str() << std::endl;
      throw s.str();
    }
  }
}

void setUniformMatrix(GLuint program_id, const char *name, const glm::mat4 &m) {
  GLuint loc;
  glm::mat4 ret;
  RETURN_IF_GL_ERROR(loc = glGetUniformLocation(program_id, name));
  RETURN_IF_GL_ERROR(glProgramUniformMatrix4fv(program_id, loc, 1, GL_FALSE,
                                               glm::value_ptr(m)));
  RETURN_IF_GL_ERROR(glGetUniformfv(program_id, loc, glm::value_ptr(ret)));
  if (ret != m) {
    std::stringstream s;
    s << "name : " << name << " loc : " << loc
      << " expected : " << glm::to_string(m)
      << " actual : " << glm::to_string(ret);
    std::cout << s.str() << std::endl;
    throw s.str();
  }
}
void setUniformMatrix(GLuint program_id, const char *name, int count,
                      const glm::mat4 m[]) {
  for (int i = 0; i < count; i++) {
    std::stringstream s;
    s << name << "[" << i << "]";
    setUniformMatrix(program_id, s.str().c_str(), m[i]);
  }
}

std::string slurp(const char *fname) {
  std::ifstream in(fname);
  std::stringstream sstr;
  sstr << in.rdbuf();
  return sstr.str();
}

void setup(Cube &c, int width, int height, GLuint &program_id, GLuint &vao,
           GLuint &vbo) {
  RETURN_IF_GL_ERROR(glEnable(GL_DEPTH_TEST));
  RETURN_IF_GL_ERROR(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

  program_id = CreateShaderProgram(slurp("ncube.vert"), slurp("ncube.geom"),
                                   slurp("ncube.frag"));
  RETURN_IF_GL_ERROR(glUseProgram(program_id));
  setUniformMatrix(program_id, "pTransMat", 26, Cube::pTransMat);
  setUniformMatrix(program_id, "pRotMat", 24, Cube::pRotMat);

  printf("program_id : %02d\n", program_id);
  // this should never change after this

  RETURN_IF_GL_ERROR(glGenVertexArrays(1, &vao));
  RETURN_IF_GL_ERROR(glBindVertexArray(vao));
  RETURN_IF_GL_ERROR(glGenBuffers(1, &vbo));
  RETURN_IF_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  RETURN_IF_GL_ERROR(
      glBufferData(GL_ARRAY_BUFFER, 78, c.renderingData, GL_DYNAMIC_DRAW));
  RETURN_IF_GL_ERROR(glBindVertexArray(0));
}

void setVertexAttrib(GLuint program_id, const char *attribName, GLvoid *ptr) {
  GLint size = 1;
  GLenum type = GL_UNSIGNED_BYTE;
  GLsizei stride = 3;
  GLuint loc;
  RETURN_IF_GL_ERROR(loc = glGetAttribLocation(program_id, attribName));
  RETURN_IF_GL_ERROR(glEnableVertexAttribArray(loc));
  RETURN_IF_GL_ERROR(glVertexAttribIPointer(loc, size, type, stride, ptr));
}

void render(Cube &c, int width, int height, GLuint program_id, GLuint vao,
            GLuint vbo) {
  std::lock_guard<std::mutex> guard(c.objectLock);
  setUniformMatrix(program_id, "camera", c.camera);
  setUniformMatrix(program_id, "projection", c.projection);
  setUniformMatrix(program_id, "cubeTranslation", c.cubeTranslation);
  setUniformMatrix(program_id, "cubeRotation", c.cubeRotation);
  setUniformMatrix(program_id, "rotatingSideTrf", c.rotatingSideTrf);
  setUniformMatrix(program_id, "pScale", c.pScale);

  RETURN_IF_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
  RETURN_IF_GL_ERROR(glViewport(0, 0, width, height));

  RETURN_IF_GL_ERROR(glUseProgram(program_id));
  RETURN_IF_GL_ERROR(glBindVertexArray(vao));
  RETURN_IF_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  RETURN_IF_GL_ERROR(
      glBufferData(GL_ARRAY_BUFFER, 78, c.renderingData, GL_DYNAMIC_DRAW));

  setVertexAttrib(program_id, "rotMatId", (void *)(0));
  setVertexAttrib(program_id, "transMatId", (void *)(1));
  setVertexAttrib(program_id, "pieceId", (void *)(2));
  RETURN_IF_GL_ERROR(
      glDrawArrays(GL_POINTS, 0 /*0*/, 26 /*26*/ /*number of verts*/));

  RETURN_IF_GL_ERROR(glBindVertexArray(0));

  fflush(stdout);
}

void envinfo() {
  const GLubyte *renderer = glGetString(GL_RENDERER);
  const GLubyte *vendor = glGetString(GL_VENDOR);
  const GLubyte *version = glGetString(GL_VERSION);
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  GLint major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  printf("GL Vendor : %s\n", vendor);
  printf("GL Renderer : %s\n", renderer);
  printf("GL Version (string) : %s\n", version);
  printf("GL Version (integer) : %d.%d\n", major, minor);
  printf("GLSL Version : %s\n", glslVersion);
}
