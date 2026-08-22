

#include "vectr.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

struct charac {
  char *c;
};

struct pixel {
  float a = 1.0f;
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
};

struct triangle {
  vec3df x, y, z, norm;
  float r, g, b, a;
  triangle(vec3df p, vec3df p1, vec3df p2, float rr, float gg, float bb,
           float aa);
};

struct Xspace {
  int res;
  int maxCha = 10;
  pixel *buffer;
  float *zbuffer;

  charac listC[10];

  Xspace(int r);
  void setCharacterDensityList();
  int convDen(float t);

  void render();
  void clear();

  void setPixel(vec2 xp, pixel cac = {1.0f, 1.0f, 1.0f, 1.0f}, float zz = 1.0f);
  // void line(vec2 p1, vec2 p2);

  void line(vec2 p1, vec2 p2, pixel pix = {1.0f, 1.0f, 1.0f, 1.0f},
            float z1 = 1.0f, float z2 = 1.0f);

  bool isInsideTriangle(vec2 cord1, vec2 cord2, vec2 cord3, vec2 point);

  void drawTriangle(const vec2 cord1, const vec2 cord2, const vec2 cord3,
                    const pixel pix = {1.0f, 1.0f, 1.0f, 1.0f}, float z1 = 1.0f,
                    float z2 = 1.0f);
};
