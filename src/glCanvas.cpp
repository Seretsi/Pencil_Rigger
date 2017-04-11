#include <cstdlib>
#include "glCanvas.h"
#include "argparser.h"
#include "camera.h"

#include <unistd.h>

#include "mesh.h"
#include "radiosity.h"
#include "photon_mapping.h"
#include "raytracer.h"
#include "raytree.h"

#include "utils.h"

// ========================================================
// static variables of GLCanvas class

ArgParser* GLCanvas::args = NULL;
Camera* GLCanvas::camera = NULL;

Mesh* GLCanvas::mesh = NULL;
RayTracer* GLCanvas::raytracer = NULL;
Radiosity* GLCanvas::radiosity = NULL;
PhotonMapping* GLCanvas::photon_mapping = NULL;

BoundingBox GLCanvas::bbox;
GLFWwindow* GLCanvas::window = NULL;

// mouse position
int GLCanvas::mouseX = 0;
int GLCanvas::mouseY = 0;
// which mouse button
bool GLCanvas::leftMousePressed = false;
bool GLCanvas::middleMousePressed = false;
bool GLCanvas::rightMousePressed = false;
// current state of modifier keys
bool GLCanvas::shiftKeyPressed = false;
bool GLCanvas::controlKeyPressed = false;
bool GLCanvas::altKeyPressed = false;
bool GLCanvas::superKeyPressed = false;

int GLCanvas::raytracing_x;
int GLCanvas::raytracing_y;
int GLCanvas::raytracing_divs_x;
int GLCanvas::raytracing_divs_y;

GLuint GLCanvas::render_VAO;

GLuint GLCanvas::ViewMatrixID;
GLuint GLCanvas::ModelMatrixID;
GLuint GLCanvas::LightID;
GLuint GLCanvas::MatrixID;
GLuint GLCanvas::programID;
GLuint GLCanvas::wireframeID;
GLuint GLCanvas::colormodeID;

GLuint GLCanvas::textureID;
GLint GLCanvas::mytexture;

// ========================================================
// Initialize all appropriate OpenGL variables, set
// callback functions, and start the main event loop.
// This function will not return but can be terminated
// by calling 'exit(0)'
// ========================================================

void GLCanvas::initialize(ArgParser *_args) {

  args = _args;
  mesh = NULL;

  glfwSetErrorCallback(error_callback);

  // Initialize GLFW
  if( !glfwInit() ) {
    std::cerr << "ERROR: Failed to initialize GLFW" << std::endl;
    exit(1);
  }
  
  // We will ask it to specifically open an OpenGL 3.2 context
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a GLFW window
  window = glfwCreateWindow(args->width,args->height, "OpenGL viewer", NULL, NULL);
  if (!window) {
    std::cerr << "ERROR: Failed to open GLFW window" << std::endl;
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  HandleGLError("in glcanvas first");

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
    glfwTerminate();
    exit(1);
  }

  // there seems to be a "GL_INVALID_ENUM" error in glewInit that is a
  // know issue, but can safely be ignored
  HandleGLError("after glewInit()",true);

  std::cout << "-------------------------------------------------------" << std::endl;
  std::cout << "OpenGL Version: " << (char*)glGetString(GL_VERSION) << '\n';
  std::cout << "-------------------------------------------------------" << std::endl;

  // Initialize callback functions
  glfwSetCursorPosCallback(GLCanvas::window,GLCanvas::mousemotionCB);
  glfwSetMouseButtonCallback(GLCanvas::window,GLCanvas::mousebuttonCB);
  glfwSetKeyCallback(GLCanvas::window,GLCanvas::keyboardCB);

  programID = LoadShaders( args->path+"/render.vertexshader",
                           args->path+"/render.fragmentshader" );

  // Load mesh
  Load();
  GLCanvas::initializeVBOs();
  GLCanvas::setupVBOs();
  camera->glPlaceCamera(); 

  HandleGLError("finished glcanvas initialize");
}


void GLCanvas::Load(){
  mesh = new Mesh();
  mesh->Load(args);
 
  raytracer = new RayTracer(mesh,args);
  radiosity = new Radiosity(mesh,args);
  photon_mapping = new PhotonMapping(mesh,args);

  raytracer->setRadiosity(radiosity);
  raytracer->setPhotonMapping(photon_mapping);
  radiosity->setRayTracer(raytracer);
  radiosity->setPhotonMapping(photon_mapping);
  photon_mapping->setRayTracer(raytracer);
  photon_mapping->setRadiosity(radiosity);

  // ===========================
  // initial placement of camera 
  assert (mesh->camera != NULL);
  camera = mesh->camera;
}


void GLCanvas::animate(){

  if (args->radiosity_animation) {
    double undistributed = radiosity->Iterate();
    if (undistributed < 0.001) {
      args->radiosity_animation = false;
      std::cout << "undistributed < 0.001, animation stopped\n"; fflush(stdout);
    }
    radiosity->setupVBOs();
  }

  if (args->raytracing_animation) {
    // draw 100 pixels and then refresh the screen and handle any user input
    for (int i = 0; i < 100; i++) {
      if (!DrawPixel()) {
        args->raytracing_animation = false;
        break;
      }
    }
    raytracer->setupVBOs();
  }

  usleep (1000);
}


void GLCanvas::initializeVBOs(){
  HandleGLError("enter initilizeVBOs()");
  glGenVertexArrays(1, &render_VAO);
  glBindVertexArray(render_VAO);

  GLCanvas::MatrixID = glGetUniformLocation(GLCanvas::programID, "MVP");
  GLCanvas::LightID = glGetUniformLocation(GLCanvas::programID, "LightPosition_worldspace");
  GLCanvas::ViewMatrixID = glGetUniformLocation(GLCanvas::programID, "V");
  GLCanvas::ModelMatrixID = glGetUniformLocation(GLCanvas::programID, "M");
  GLCanvas::wireframeID = glGetUniformLocation(GLCanvas::programID, "wireframe");
  GLCanvas::colormodeID = glGetUniformLocation(GLCanvas::programID, "colormode");
  // FIXME: texture still buggy
  GLCanvas::mytexture = glGetUniformLocation(GLCanvas::programID, "mytexture");

  bbox.initializeVBOs();
  RayTree::initializeVBOs();
  radiosity->initializeVBOs();
  raytracer->initializeVBOs();
  photon_mapping->initializeVBOs();

  HandleGLError("leaving initilizeVBOs()");
}


void GLCanvas::setupVBOs(){
  HandleGLError("enter GLCanvas::setupVBOs()");
  bbox.Set(*mesh->getBoundingBox());
  bbox.setupVBOs();
  radiosity->setupVBOs();
  photon_mapping->setupVBOs();
  HandleGLError("leaving GLCanvas::setupVBOs()");
}


void GLCanvas::drawVBOs(const glm::mat4 &ProjectionMatrix,const glm::mat4 &ViewMatrix,const glm::mat4 &ModelMatrix){
  HandleGLError("enter GlCanvas::drawVBOs()");
  if (!args->intersect_backfacing) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  } else {    
    glDisable(GL_CULL_FACE);
  }
  // prepare data to send to the shaders
  glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  glm::vec3 lightPos = glm::vec3(4,4,4);
  glUniform3f(GLCanvas::LightID, lightPos.x, lightPos.y, lightPos.z);
  glUniformMatrix4fv(GLCanvas::MatrixID, 1, GL_FALSE, &MVP[0][0]);
  glUniformMatrix4fv(GLCanvas::ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniformMatrix4fv(GLCanvas::ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
  glUniform1i(GLCanvas::wireframeID, args->wireframe);
  glUniform1i(GLCanvas::colormodeID, 0);
  glUniform1i(GLCanvas::mytexture, /*GL_TEXTURE*/0);
  radiosity->drawVBOs();
  photon_mapping->drawVBOs();
  RayTree::drawVBOs();
  if (args->intersect_backfacing) {
    glDisable(GL_CULL_FACE);
  }
  raytracer->drawVBOs();
  if (args->intersect_backfacing) {
    glEnable(GL_CULL_FACE);
  }
  HandleGLError("leaving GlCanvas::drawVBOs()");
}


void GLCanvas::cleanupVBOs(){
  radiosity->cleanupVBOs();
  photon_mapping->cleanupVBOs();  
  raytracer->cleanupVBOs();  
  RayTree::cleanupVBOs();  
  bbox.cleanupVBOs();
}


// ========================================================
// Callback function for mouse click or release
// ========================================================

void GLCanvas::mousebuttonCB(GLFWwindow *window, int which_button, int action, int mods) {
  // store the current state of the mouse buttons
  if (which_button == GLFW_MOUSE_BUTTON_1) {
    if (action == GLFW_PRESS) {
      leftMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      leftMousePressed = false;
    }
  } else if (which_button == GLFW_MOUSE_BUTTON_2) {
    if (action == GLFW_PRESS) {
      rightMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      rightMousePressed = false;
    }
  } else if (which_button == GLFW_MOUSE_BUTTON_3) {
    if (action == GLFW_PRESS) {
      middleMousePressed = true;
    } else {
      assert (action == GLFW_RELEASE);
      middleMousePressed = false;
    }
  }
}	

// ========================================================
// Callback function for mouse drag
// ========================================================

void GLCanvas::mousemotionCB(GLFWwindow *window, double x, double y) {

  // stop raytracing if the camera has moved
  if ( (leftMousePressed || middleMousePressed || rightMousePressed) 
       && args->raytracing_animation) {
    printf ("raytracing animation stopped, press 'R' to start\n");    
    args->raytracing_animation = false;
  }

  // camera controls that work well for a 3 button mouse
  if (!shiftKeyPressed && !controlKeyPressed && !altKeyPressed) {
    if (leftMousePressed) {
      camera->rotateCamera(mouseX-x,mouseY-y);
    } else if (middleMousePressed)  {
      camera->truckCamera(mouseX-x, y-mouseY);
    } else if (rightMousePressed) {
      camera->dollyCamera(mouseY-y);
    }
  }

  if (leftMousePressed || middleMousePressed || rightMousePressed) {
    if (shiftKeyPressed) {
      camera->zoomCamera(mouseY-y);
    }
    // allow reasonable control for a non-3 button mouse
    if (controlKeyPressed) {
      camera->truckCamera(mouseX-x, y-mouseY);    
    }
    if (altKeyPressed) {
      camera->dollyCamera(y-mouseY);    
    }
  }
  mouseX = x;
  mouseY = y;
}

// ========================================================
// Callback function for keyboard events
// ========================================================

void GLCanvas::keyboardCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // store the modifier keys
  shiftKeyPressed = (GLFW_MOD_SHIFT & mods);
  controlKeyPressed = (GLFW_MOD_CONTROL & mods);
  altKeyPressed = (GLFW_MOD_ALT & mods);
  superKeyPressed = (GLFW_MOD_SUPER & mods);
  // non modifier key actions

  if (key == GLFW_KEY_ESCAPE || key == 'q' || key == 'Q') {
    glfwSetWindowShouldClose(GLCanvas::window, GL_TRUE);
  }

  // other normal ascii keys...
  if ( (action == GLFW_PRESS || action == GLFW_REPEAT) && key < 256) {
    switch (key) {
    // RAYTRACING STUFF
    case 'r':  case 'R':  case 'g':  case 'G': { 
      args->raytracing_animation = !args->raytracing_animation;
      glfwGetWindowSize(window, &args->width, &args->height);
      // animate raytracing of the scene
      if (args->raytracing_animation) {
        if (key == 'r' || key == 'R') {
          args->gather_indirect = false;
          printf ("raytracing animation started, press 'R' to stop\n");
        } else {
          args->gather_indirect = true; 
          printf ("photon mapping animation started, press 'G' to stop\n");
        }
        if (args->width <= args->height) {
          raytracing_divs_x = 10;
          raytracing_divs_y = 10 * args->height / float (args->width);
        } else {
          raytracing_divs_x = 10 * args->width / float (args->height);
          raytracing_divs_y = 10;
        }
        raytracing_x = 0;
        raytracing_y = 0;
        raytracer->resetVBOs();
      } else
        printf ("raytracing animation stopped, press 'R' to start\n");    
      break;
    }
    case 't':  case 'T': {
      // visualize the ray tree for the pixel at the current mouse position
      glfwGetWindowSize(window, &args->width, &args->height);
      RayTree::Activate();
      raytracing_divs_x = -1;
      raytracing_divs_y = -1;
      TraceRay(mouseX,args->height-mouseY);
      RayTree::Deactivate();
      glm::vec3 cp = camera->camera_position;
      glm::vec3 poi = camera->point_of_interest;
      float distance = glm::length((cp-poi)/2.0f);
      RayTree::setupVBOs(distance / 500.0); 
      radiosity->setupVBOs();
      photon_mapping->setupVBOs();
      break; }
    case 'l':  case 'L': { 
      // toggle photon rendering
      args->render_photons = !args->render_photons;
      break; }
    case 'k':  case 'K': { 
      // toggle photon rendering
      args->render_kdtree = !args->render_kdtree;
      break; }
    case 'p':  case 'P': { 
      // toggle photon rendering
      photon_mapping->TracePhotons();
      photon_mapping->setupVBOs();
      break; }
    
      // RADIOSITY STUFF
    case ' ': 
      // a single step of radiosity
      radiosity->Iterate();
      radiosity->setupVBOs();
      break;
    case 'a': case 'A':
      // animate radiosity solution
      args->radiosity_animation = !args->radiosity_animation;
      if (args->radiosity_animation) 
        printf ("radiosity animation started, press 'A' to stop\n");
      else
        printf ("radiosity animation stopped, press 'A' to start\n");
      break;
    case 's': case 'S':
      // subdivide the mesh for radiosity
      radiosity->Cleanup();
      radiosity->getMesh()->Subdivision();
      radiosity->Reset();
      radiosity->setupVBOs();
      break;
    case 'c': case 'C':
      // clear the raytracing visualization
      args->raytracing_animation = false;
      raytracer->resetVBOs();
      raytracer->setupVBOs();
      // clear the radiosity solution
      args->radiosity_animation = false;
      radiosity->Reset();
      radiosity->setupVBOs();
      break;
      
      // VISUALIZATIONS
    case 'w':  case 'W':
      // render wireframe mode
      args->wireframe = !args->wireframe;
      break;
    case 'v': case 'V':
      // toggle the different visualization modes
      args->render_mode = RENDER_MODE((args->render_mode+1)%NUM_RENDER_MODES);
      switch (args->render_mode) {
      case RENDER_MATERIALS: std::cout << "RENDER_MATERIALS\n"; fflush(stdout); break;
      case RENDER_LIGHTS: std::cout << "RENDER_LIGHTS\n"; fflush(stdout); break;
      case RENDER_UNDISTRIBUTED: std::cout << "RENDER_UNDISTRIBUTED\n"; fflush(stdout); break;
      case RENDER_ABSORBED: std::cout << "RENDER_ABSORBED\n"; fflush(stdout); break;
      case RENDER_RADIANCE: std::cout << "RENDER_RADIANCE\n"; fflush(stdout); break;
      case RENDER_FORM_FACTORS: std::cout << "RENDER_FORM_FACTORS\n"; fflush(stdout); break;
      default: assert(0); }
      radiosity->setupVBOs();
      break;
    case 'i':  case 'I':
      // interpolate patch illumination values
      args->interpolate = !args->interpolate;
      radiosity->setupVBOs();
      break;
    case 'b':  case 'B':
      // interpolate patch illumination values
      args->intersect_backfacing = !args->intersect_backfacing;
      break;
      
    case 'x':  case 'X':
      std::cout << "CURRENT CAMERA" << std::endl;
      std::cout << *camera << std::endl;
      break;

    case 'q':  case 'Q':
      // quit
      glfwSetWindowShouldClose(GLCanvas::window, GL_TRUE);
      break;
    default:
      std::cout << "UNKNOWN KEYBOARD INPUT  '" << (char)key << "'" << std::endl;
    }
    setupVBOs();
  }
}




// trace a ray through pixel (i,j) of the image an return the color
glm::vec3 GLCanvas::TraceRay(double i, double j) {

  // compute and set the pixel color
  int max_d = std::max(args->width,args->height);
  glm::vec3 color;
  



  // ==================================
  // ASSIGNMENT: IMPLEMENT ANTIALIASING
  // ==================================
  


  // Here's what we do with a single sample per pixel:
  // construct & trace a ray through the center of the pixle
  double x = (i-args->width/2.0)/double(max_d)+0.5;
  double y = (j-args->height/2.0)/double(max_d)+0.5;
  Ray r = camera->generateRay(x,y); 
  Hit hit;
  color = raytracer->TraceRay(r,hit,args->num_bounces);
  // add that ray for visualization
  RayTree::AddMainSegment(r,0,hit.getT());




  // return the color
  return color;
}




// for visualization: find the "corners" of a pixel on an image plane
// 1/2 way between the camera & point of interest
glm::vec3 GLCanvas::GetPos(double i, double j) {
  int max_d = std::max(args->width,args->height);
  double x = (i-args->width/2.0)/double(max_d)+0.5;
  double y = (j-args->height/2.0)/double(max_d)+0.5;
  Ray r = camera->generateRay(x,y); 
  glm::vec3 cp = camera->camera_position;
  glm::vec3 poi = camera->point_of_interest;
  float distance = glm::length((cp-poi)/2.0f);
  return r.getOrigin()+distance*r.getDirection();
}


// Scan through the image from the lower left corner across each row
// and then up to the top right.  Initially the image is sampled very
// coarsely.  Increment the static variables that track the progress
// through the scans
int GLCanvas::DrawPixel() {
  if (raytracing_x >= raytracing_divs_x) {
    // end of row
    raytracing_x = 0; 
    raytracing_y += 1;
  }
  if (raytracing_y >= raytracing_divs_y) {
    // last row
    if (raytracing_divs_x >= args->width ||
        raytracing_divs_y >= args->height) {
      // stop rendering, matches resolution of current camera
      return 0; 
    }
    // else decrease pixel size & start over again in the bottom left corner
    raytracing_divs_x *= 3;
    raytracing_divs_y *= 3;
    if (raytracing_divs_x > args->width * 0.51 ||
        raytracing_divs_x > args->height * 0.51) {
      raytracing_divs_x = args->width;
      raytracing_divs_y = args->height;
    }
    raytracing_x = 0;
    raytracing_y = 0;
    if (raytracer->render_to_a) {
      raytracer->pixels_b.clear();
      raytracer->pixels_indices_b.clear();
     raytracer->render_to_a = false;
    } else {
      raytracer->pixels_a.clear();
      raytracer->pixels_indices_a.clear();
      raytracer->render_to_a = true;
    }
  }

  double x_spacing = args->width / double (raytracing_divs_x);
  double y_spacing = args->height / double (raytracing_divs_y);

  // compute the color and position of intersection
  glm::vec3 pos1 =  GetPos((raytracing_x  )*x_spacing, (raytracing_y  )*y_spacing);
  glm::vec3 pos2 =  GetPos((raytracing_x+1)*x_spacing, (raytracing_y  )*y_spacing);
  glm::vec3 pos3 =  GetPos((raytracing_x+1)*x_spacing, (raytracing_y+1)*y_spacing);
  glm::vec3 pos4 =  GetPos((raytracing_x  )*x_spacing, (raytracing_y+1)*y_spacing);
  
  glm::vec3 color = TraceRay((raytracing_x+0.5)*x_spacing, (raytracing_y+0.5)*y_spacing);

  double r = linear_to_srgb(color.r);
  double g = linear_to_srgb(color.g);
  double b = linear_to_srgb(color.b);
  if (raytracer->render_to_a) {
    int start = raytracer->pixels_a.size();
    raytracer->pixels_a.push_back(VBOPosNormalColor(pos1,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_a.push_back(VBOPosNormalColor(pos2,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_a.push_back(VBOPosNormalColor(pos3,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_a.push_back(VBOPosNormalColor(pos4,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_indices_a.push_back(VBOIndexedTri(start+0,start+1,start+2));
    raytracer->pixels_indices_a.push_back(VBOIndexedTri(start+0,start+2,start+3));
  } else {
    int start = raytracer->pixels_b.size();
    raytracer->pixels_b.push_back(VBOPosNormalColor(pos1,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_b.push_back(VBOPosNormalColor(pos2,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_b.push_back(VBOPosNormalColor(pos3,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_b.push_back(VBOPosNormalColor(pos4,glm::vec3(0,0,0),glm::vec4(r,g,b,1.0)));
    raytracer->pixels_indices_b.push_back(VBOIndexedTri(start+0,start+1,start+2));
    raytracer->pixels_indices_b.push_back(VBOIndexedTri(start+0,start+2,start+3));
  }  

  raytracing_x += 1;
  return 1;
}


// ========================================================
// Load the vertex & fragment shaders
// ========================================================

GLuint LoadShaders(const std::string &vertex_file_path,const std::string &fragment_file_path){

  std::cout << "load shaders" << std::endl;

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  
  // Read the Vertex Shader code from the file
  std::string VertexShaderCode;
  std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
  if (VertexShaderStream.is_open()){
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
      VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  } else {
    std::cerr << "ERROR: cannot open " << vertex_file_path << std::endl;
    exit(0);
  }
  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
      FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  } else {
    std::cerr << "ERROR: cannot open " << vertex_file_path << std::endl;
    exit(0);
  }
  
  GLint Result = GL_FALSE;
 
  // Compile Vertex Shader
  std::cout << "Compiling shader : " << vertex_file_path << std::endl;
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);
 
  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  if (Result != GL_TRUE) {  
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(VertexShaderID, 1024, &log_length, message);
    std::cout << "VERTEX ERROR " << message << std::endl;
    exit(1);
  }

  // Compile Fragment Shader
  std::cout << "Compiling shader : " << fragment_file_path << std::endl;
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);
 
  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  if (Result != GL_TRUE) {  
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(FragmentShaderID, 1024, &log_length, message);
    std::cout << "FRAGMENT ERROR " << message << std::endl;
    exit(1);
  }

  // Link the program
  std::cout << "Linking shader program" << std::endl;
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);
 
  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  if (Result != GL_TRUE) {  
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(ProgramID, 1024, &log_length, message);
    std::cout << "SHADER PROGRAM ERROR " << message << std::endl;
    exit(1);
  }

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

// ========================================================
// Functions related to error handling
// ========================================================

void GLCanvas::error_callback(int error, const char* description) {
  std::cerr << "ERROR CALLBACK: " << description << std::endl;
}

std::string WhichGLError(GLenum &error) {
  switch (error) {
  case GL_NO_ERROR:
    return "NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  case GL_STACK_UNDERFLOW:
    return "GL_STACK_UNDERFLOW";
  case GL_STACK_OVERFLOW:
    return "GL_STACK_OVERFLOW";
  default:
    return "OTHER GL ERROR";
  }
}

int HandleGLError(const std::string &message, bool ignore) {
  GLenum error;
  int i = 0;
  while ((error = glGetError()) != GL_NO_ERROR) {
    if (!ignore) {
      if (message != "") {
	std::cerr << "[" << message << "] ";
      }
      std::cerr << "GL ERROR(" << i << ") " << WhichGLError(error) << std::endl;
    }
    i++;
  }
  if (i == 0) return 1;
  return 0;
}

// ========================================================
// ========================================================
