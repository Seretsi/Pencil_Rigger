#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include <random>

// VISUALIZATION MODES FOR RADIOSITY
#define NUM_RENDER_MODES 6
enum RENDER_MODE { RENDER_MATERIALS, RENDER_RADIANCE, RENDER_FORM_FACTORS, 
		   RENDER_LIGHTS, RENDER_UNDISTRIBUTED, RENDER_ABSORBED };

// ================================================================================
// ================================================================================

inline void separatePathAndFile(const std::string &input, std::string &path, std::string &file) {
  // we need to separate the filename from the path
  // (we assume the vertex & fragment shaders are in the same directory)
  // first, locate the last '/' in the filename
  size_t last = std::string::npos;  
  while (1) {
    int next = input.find('/',last+1);
    if (next != (int)std::string::npos) { 
      last = next;
      continue;
    }
    next = input.find('\\',last+1);
    if (next != (int)std::string::npos) { 
      last = next;
      continue;
    }
    break;
  }
  if (last == std::string::npos) {
    // if there is no directory in the filename
    file = input;
    path = ".";
  } else {
    // separate filename & path
    file = input.substr(last+1,input.size()-last-1);
    path = input.substr(0,last);
  }
}

// ======================================================================
// Class to collect all the high-level rendering parameters controlled
// by the command line or the keyboard input
// ======================================================================

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {
    DefaultValues();
    // parse the command line arguments
    for (int i = 1; i < argc; i++) {
      if (std::string(argv[i]) == std::string("-input") || 
          std::string(argv[i]) == std::string("-i")) {
        i++; assert (i < argc); 
        separatePathAndFile(argv[i],path,input_file);
      } else if (std::string(argv[i]) == std::string("-size")) {
	i++; assert (i < argc); 
	width = atoi(argv[i]);
	i++; assert (i < argc); 
         height = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-num_form_factor_samples")) {
	i++; assert (i < argc); 
	num_form_factor_samples = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-sphere_rasterization")) {
	i++; assert (i < argc); 
	sphere_horiz = atoi(argv[i]);
         if (sphere_horiz % 2 == 1) sphere_horiz++; 
	i++; assert (i < argc); 
	sphere_vert = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-cylinder_ring_rasterization")) {
	i++; assert (i < argc); 
	cylinder_ring_rasterization = atoi(argv[i]);

      } else if (std::string(argv[i]) == std::string("-num_bounces")) {
	i++; assert (i < argc); 
	num_bounces = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-num_shadow_samples")) {
	i++; assert (i < argc); 
	num_shadow_samples = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-num_antialias_samples")) {
	i++; assert (i < argc); 
	num_antialias_samples = atoi(argv[i]);
	assert (num_antialias_samples > 0);
      } else if (std::string(argv[i]) == std::string("-num_glossy_samples")) {
	i++; assert (i < argc); 
	num_glossy_samples = atoi(argv[i]);
	assert (num_glossy_samples > 0);
      } else if (std::string(argv[i]) == std::string("-ambient_light")) {
	i++; assert (i < argc);
	float r = atof(argv[i]);
	i++; assert (i < argc);
	float g = atof(argv[i]);
	i++; assert (i < argc);
	float b = atof(argv[i]);
	ambient_light = glm::vec3(r,g,b);
      } else if (std::string(argv[i]) == std::string("-num_photons_to_shoot")) {
	i++; assert (i < argc);
	num_photons_to_shoot = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-num_photons_to_collect")) {
	i++; assert (i < argc);
	num_photons_to_collect = atoi(argv[i]);
      } else if (std::string(argv[i]) == std::string("-gather_indirect")) {
	gather_indirect = true;
      } else {
	std::cout << "ERROR: unknown command line argument " 
		  << i << ": '" << argv[i] << "'" << std::endl;
	exit(1);
      }
    }
  }

  double rand() {
#if 1
    // random seed
    static std::random_device rd;    
    static std::mt19937 engine(rd());
#else
    // deterministic randomness
    static std::mt19937 engine(37);
#endif
    static std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine);
  }

  void DefaultValues() {
    // BASIC RENDERING PARAMETERS
    input_file = "";
    path = "";
    width = 500;
    height = 500;
    raytracing_animation = false;
    radiosity_animation = false;

    // RADIOSITY PARAMETERS
    render_mode = RENDER_MATERIALS;
    interpolate = false;
    wireframe = false;
    num_form_factor_samples = 1;
    sphere_horiz = 8;
    sphere_vert = 6;
    cylinder_ring_rasterization = 20; 

    // RAYTRACING PARAMETERS
    num_bounces = 0;
    num_shadow_samples = 0;
    num_antialias_samples = 1;
    num_glossy_samples = 1;
    ambient_light = glm::vec3(0.1,0.1,0.1);
    intersect_backfacing = false;

    // PHOTON MAPPING PARAMETERS
    render_photons = true;
    render_kdtree = true;
    num_photons_to_shoot = 10000;
    num_photons_to_collect = 100;
    gather_indirect = false;
  }

  // ==============
  // REPRESENTATION
  // all public! (no accessors)

  // BASIC RENDERING PARAMETERS
  std::string input_file;
  std::string path;
  int width;
  int height;
  bool raytracing_animation;
  bool radiosity_animation;

  // RADIOSITY PARAMETERS
  enum RENDER_MODE render_mode;
  bool interpolate;
  bool wireframe;
  int num_form_factor_samples;
  int sphere_horiz;
  int sphere_vert;
  int cylinder_ring_rasterization;

  // RAYTRACING PARAMETERS
  int num_bounces;
  int num_shadow_samples;
  int num_antialias_samples;
  int num_glossy_samples;
  glm::vec3 ambient_light;
  bool intersect_backfacing;

  // PHOTON MAPPING PARAMETERS
  int num_photons_to_shoot;
  int num_photons_to_collect;
  bool render_photons;
  bool render_kdtree;
  bool gather_indirect;

};

#endif
