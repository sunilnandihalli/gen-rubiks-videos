
#include "ncube_renderer.h"
#include "rubutils.h"

#include "Cube.h"
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void saveFrame(int width,int height,int frameId,const char* prefix);
int main() {
  Cube::init();
  Cube c;
  GLuint program_id, vao, vbo, width(800), height(600);
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this
  // statement to fix compilation on OS X

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  Cube::init();
  // SETUP
  //  c.animate();
  setup(c, width, height, program_id, vao, vbo);
  int numRenders = 0;
  // render loop
  AnimationData *animData = animationSetup(c);
  while (!glfwWindowShouldClose(window) && nextFrame(animData)) {
    processInput(window);
    render(c, width, height, program_id, vao, vbo);
    saveFrame(width,height,numRenders,"runs/01/");
    numRenders += 1;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
#include <netpbm/pam.h>
void saveFrame(int width,int height,int frameId,const char* prefix) {
  struct pam inpam, outpam;
  tuple * tuplerow;
  unsigned int row;
  pm_init(argv[0], 0);
  pnm_readpaminit(stdin, &inpam, PAM_STRUCT_SIZE(tuple_type));
  outpam = inpam; outpam.file = stdout;
  pnm_writepaminit(&outpam);
  tuplerow = pnm_allocpamrow(&inpam);

  for (row = 0; row < inpam.height; ++row) {
    unsigned int column;
    pnm_readpamrow(&inpam, tuplerow);
    for (column = 0; column < inpam.width; ++column) {
      unsigned int plane;
      for (plane = 0; plane < inpam.depth; ++plane) {
	grand_total += tuplerow[column][plane];
      }
    }
    pnm_writepamrow(&outpam, tuplerow);
  }
  pnm_freepamrow(tuplerow);
  
}
