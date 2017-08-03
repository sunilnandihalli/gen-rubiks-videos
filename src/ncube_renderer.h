#ifndef NCUBE_RENDERER_H_
#define NCUBE_RENDERER_H_
#include "Cube.h"
#include "rubutils.h"

#define GL_GLEXT_LEGACY

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

void setUniformData(GLuint program_id, const char *name, int size, const int *data) ;
void setUniformMatrix(GLuint program_id, const char *name, const glm::mat4 &m) ;
void setUniformMatrix(GLuint program_id, const char *name, int count, const glm::mat4 m[]) ;
void setup(Cube& c, int width, int height, GLuint& program_id, GLuint& vao, GLuint& vbo) ;
void setVertexAttrib(const char *attribName, GLvoid *ptr);
void render(Cube& c, int width,int height, GLuint program_id, GLuint vao, GLuint vbo) ;
#endif  
