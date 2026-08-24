#pragma once
#include "vectr.h"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <vector>

struct charac {
  std::string c;
};

struct color {
  float a;
  float r;
  float g;
  float b;
  float alpha;
};

struct pixel {
  float a = 1.0f;
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float alpha = 1.0f;
};

struct Xspace {
  vec2 res = {32, 32};

  // const int maxCha = 10;
  std::vector<charac> listC;

  pixel *buffer;
  float *zbuffer;

  Xspace(vec2 res);
  Xspace(int res);

  void setCharacterDensityList();

  int convDen(float t);

  void render();
  void clear();

  void setPixel(vec2 xp, pixel cac = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
                float zz = 1.0f, bool invert = true);

  // void line(vec2 p1, vec2 p2);

  void line(vec2 p1, vec2 p2, pixel pix = {1.0f, 1.0f, 1.0f, 1.0f},
            float z1 = 1.0f, float z2 = 1.0f);

  bool isInsideTriangle(vec2 cord1, vec2 cord2, vec2 cord3, vec2 point);

  void drawTriangle(const vec2 cord1, const vec2 cord2, const vec2 cord3,
                    const pixel pix = {1.0f, 1.0f, 1.0f, 1.0f}, float z1 = 1.0f,
                    float z2 = 1.0f);
};

// 💀 slaughterhouse

struct xframe {
  Xspace *env;
  int zindex = 0;
  float opacity = 1.0f;
  float saturation = 1.0f;
  vec2df rotation = {0, 0};
  vec2 positon = {0, 0};
  vec2 size = {0, 0};
  xframe(Xspace *e);
};

struct space {
  Xspace finalrenderimg;
  std::vector<xframe *> frames;
  vec2 res = {32, 32};

  void Transformation();
  void render();

  void addlayer(xframe *frame);

  space(vec2 res1);
};
