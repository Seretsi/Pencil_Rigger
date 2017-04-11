#ifndef _GL_CANVAS_H_
#define _GL_CANVAS_H_

// Graphics Library Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <string>

#include "boundingbox.h"

class ArgParser;
class Mesh;
class RayTracer;
class Radiosity;
class PhotonMapping;
class Camera;

// ====================================================================
// NOTE:  All the methods and variables of this class are static
// ====================================================================

class GLCanvas {

public:

  // various static variables
  static ArgParser *args;

  static Mesh *mesh;
  static RayTracer *raytracer;
  static Radiosity *radiosity;
  static PhotonMapping *photon_mapping;

  static BoundingBox bbox;
  static Camera* camera;
  static GLFWwindow* window;

  static GLuint ViewMatrixID;
  static GLuint ModelMatrixID;
  static GLuint LightID;
  static GLuint MatrixID;
  static GLuint programID;
  static GLuint colormodeID;
  static GLuint wireframeID;

  static GLuint textureID;
  static GLint mytexture;

  // mouse position
  static int mouseX;
  static int mouseY;
  // which mouse button
  static bool leftMousePressed;
  static bool middleMousePressed;
  static bool rightMousePressed;
  // current state of modifier keys
  static bool shiftKeyPressed;
  static bool controlKeyPressed;
  static bool altKeyPressed;
  static bool superKeyPressed;


  // SPECIFIC TO HW3
  static int raytracing_x;
  static int raytracing_y;
  static int raytracing_divs_x;
  static int raytracing_divs_y;


  static GLuint render_VAO;

  static void initialize(ArgParser *_args);
  static void Load();
  static void initializeVBOs();
  static void setupVBOs();
  static void drawVBOs(const glm::mat4 &ProjectionMatrix,const glm::mat4 &ViewMatrix,const glm::mat4 &ModelMatrix);
  static void cleanupVBOs();

  static void animate();

  static int DrawPixel();
  static glm::vec3 TraceRay(double i, double j);
  static glm::vec3 GetPos(double i, double j);

  // Callback functions for mouse and keyboard events
  static void mousebuttonCB(GLFWwindow *window, int which_button, int action, int mods);
  static void mousemotionCB(GLFWwindow *window, double x, double y);
  static void keyboardCB(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void error_callback(int error, const char* description);
};

// ====================================================================

// helper functions
GLuint LoadShaders(const std::string &vertex_file_path,const std::string &fragment_file_path);
std::string WhichGLError(GLenum &error);
int HandleGLError(const std::string &message = "", bool ignore = false);

#endif
