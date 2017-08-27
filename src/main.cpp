
#include "ncube_renderer.h"
#include "rubutils.h"
#include <stdio.h>
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
FILE *avconv = NULL;

/* initialize */
avconv = popen("avconv -y -f rawvideo -s 800x600 -pix_fmt rgb24 -r 25 -i - -vf vflip -an -b:v 1000k test.mp4", "w");
 if(!avconv)
   std::cout<<" popen unsuccessful"<<std::endl;
  while (!glfwWindowShouldClose(window) && nextFrame(animData) && avconv) {
    processInput(window);
    render(c, width, height, program_id, vao, vbo);
    //  saveFrame(width,height,numRenders,"runs/01/");
    unsigned char pixels[800*600*3];
    glReadPixels(0, 0, 800, 600, GL_RGB, GL_UNSIGNED_BYTE, pixels);
if (avconv)
  fwrite(pixels ,800*600*3 , 1, avconv);
 else
   std::cout<<" didn't write frame "<<std::endl;
    numRenders += 1;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
if (avconv)
  pclose(avconv);

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

