cmake_minimum_required (VERSION 2.8)
project (hw3)

set(my_executable rigger)

# toggle for building a 32 bit version (for Dr. Memory)
set(BUILD_32 "")
#set(BUILD_32 " -m32 ")

# all the .cpp files that make up this project
add_executable(${my_executable}
  main.cpp
  camera.cpp
  glCanvas.cpp
  mesh.cpp
  edge.cpp
  radiosity.cpp
  face.cpp
  raytree.cpp
  raytracer.cpp
  sphere.cpp
  cylinder_ring.cpp
  material.cpp
  image.cpp
  photon_mapping.cpp
  kdtree.cpp
  argparser.h
  boundingbox.h
  boundingbox.cpp
  camera.h
  cylinder_ring.h
  edge.h
  face.h
  glCanvas.h
  hash.h
  hit.h
  image.h
  kdtree.h
  material.h
  mesh.h
  photon.h
  photon_mapping.h
  primitive.h
  radiosity.h
  ray.h
  raytracer.h
  raytree.h
  sphere.h
  utils.h
  utils.cpp
  vertex.h
  joint.h
  Joint.cpp
)

# http://glm.g-truc.net/0.9.5/updates.html
add_definitions(-DGLM_FORCE_RADIANS)

# We've placed a few FindXXX.cmake files in the the source directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

# Following the instructions for Windows here:
# http://www.cs.rpi.edu/~cutler/classes/advancedgraphics/S17/opengl_install_notes.php
# the graphics librarys files are placed in this directory
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:\\GraphicsLibraries")

# make sure all of the necessary graphics libraries are available
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIRS})
  target_link_libraries(${my_executable} ${OPENGL_LIBRARIES} )
endif(OPENGL_FOUND)
find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIRS})
  target_link_libraries(${my_executable} ${GLEW_LIBRARIES})
endif(GLEW_FOUND)
find_package(GLM REQUIRED)
if(GLM_FOUND)
  include_directories(${GLM_INCLUDE_DIRS})
endif()
# find all the dependencies of GLFW
set(ENV{PKG_CONFIG_PATH} /usr/local/lib/pkgconfig:/usr/lib/pkgconfig:$ENV{PKG_CONFIG_PATH})
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_search_module(GLFW REQUIRED glfw3)
  include_directories(${GLFW_INCLUDE_DIRS})
else(PKG_CONFIG_FOUND)
  message("Did not find pkg-config, trying FindGLFW.cmake")
  find_package(GLFW REQUIRED)
  if(GLFW_FOUND)
    include_directories(${GLFW_INCLUDE_DIR})
  else(GLFW_FOUND)
  endif(GLFW_FOUND)
endif(PKG_CONFIG_FOUND)
message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
message(STATUS "GLEW_LIBRARIES: ${GLEW_LIBRARIES}")
message(STATUS "GLFW_LIBRARIES: ${GLFW_LIBRARIES}")
message(STATUS "GLFW_STATIC_LIBRARIES: ${GLFW_STATIC_LIBRARIES}")
message(STATUS "GLFW_LDFLAGS: ${GLFW_LDFLAGS}")
message(STATUS "GLFW_STATIC_LDFLAGS: ${GLFW_STATIC_LDFLAGS}")


# some linux compilations require this hack to get the libraries in the right order
if (APPLE)
else()
  if (UNIX)
    set(MISSING_FLAGS "-lX11 -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor -lrt -ldl")
    target_link_libraries(${my_executable} "${OPENGL_gl_LIBRARY}" "${GLEW_LIBRARIES}"  "${GLFW_LIBRARIES}" "${MISSING_FLAGS}")
  endif()
endif()


target_link_libraries(${my_executable} "${OPENGL_gl_LIBRARY}" "${GLEW_LIBRARIES}" "${GLFW_LIBRARIES}")
## this will hopefully work whether you have the static or the dynamic GLFW libraries
# string replace hack to fix a bug in the pkg_config information
string(REPLACE ";" " " flags_static "${GLFW_STATIC_LDFLAGS}")
string(REPLACE ";" " " flags_dynamic "${GLFW_LDFLAGS}")
set_property(TARGET ${my_executable} APPEND_STRING PROPERTY LINK_FLAGS "${flags_static} ${flags_dynamic}")


# platform specific compiler flags to output all compiler warnings
if (APPLE)
  # MAC OSX
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set_target_properties (${my_executable} PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic ${BUILD_32}")
  set_property(TARGET ${my_executable} APPEND_STRING PROPERTY LINK_FLAGS "${BUILD_32}")
else()
  if (UNIX)
    # LINUX
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    set_target_properties (${my_executable} PROPERTIES COMPILE_FLAGS "-g -Wall -pedantic ${BUILD_32}")
  else()
    # WINDOWS
    set_target_properties (${my_executable} PROPERTIES COMPILE_FLAGS "/W4")
  endif()
endif()
