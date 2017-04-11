#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cassert>
#include <string>
#include <iostream>

// ====================================================================
// 24 bit color
class Color {
public:
  Color(int r_=255, int g_=255, int b_=255) : r(r_),g(g_),b(b_) {}
  bool isWhite() const { return r==255 && g==255 && b==255; }
  int r,g,b;
};

// ====================================================================
// ====================================================================
// save and load from the .ppm image file format (not fully compliant)

class Image {
public:
  // ========================
  // CONSTRUCTOR & DESTRUCTOR
  Image(const std::string &filename = "") : 
  width(0), height(0), data(NULL), gl_data(NULL) {
    if (filename != "") Load(filename); 
  }
  void Allocate(int w, int h) {
    width = w;
    height = h;
    delete [] data;
    delete [] gl_data;
    gl_data = NULL;
    if (width == 0 && height == 0) {
      data = NULL;
    } else {
      assert (width > 0 && height > 0);
      data = new Color[width*height]; 
    }
  }
  ~Image() {
    delete [] data; 
    delete [] gl_data;
  }

  Image(const Image &image) { 
    copy_helper(image); }
  const Image& operator=(const Image &image) { 
    if (this != &image)
      copy_helper(image);
    return *this; }

  void copy_helper(const Image &image) {
    Allocate (image.Width(), image.Height());
    for (int i = 0; i < image.Width(); i++) {
      for (int j = 0; j < image.Height(); j++) {
        this->SetPixel(i,j,image.GetPixel(i,j));
      }
    }
  }

  // =========
  // ACCESSORS
  int Width() const { return width; }
  int Height() const { return height; }
  const Color& GetPixel(int x, int y) const {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    return data[y*width + x]; }
  // for use with OpenGL (not const because it builds gl_data first)
  unsigned char* getGLPixelData();

  // =========
  // MODIFIERS
  void SetAllPixels(const Color &value) {
    for (int i = 0; i < width*height; i++) {
      data[i] = value; } }
  void SetPixel(int x, int y, const Color &value) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    data[y*width + x] = value; }

  // ===========
  // LOAD & SAVE
  bool Load(const std::string &filename);
  bool Save(const std::string &filename) const; 
  
private:
  // ==============
  // REPRESENTATION
  int width;
  int height;
  Color *data;
  unsigned char* gl_data;
};

#endif
