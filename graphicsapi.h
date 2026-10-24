#pragma once
#include "vectr.h"
#include <functional>
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

  void increment(color A) {
    a += A.a;
    r += A.r;
    g += A.g;
    b += A.b;
    alpha += A.alpha;
  };
  color operator+(color A) {
    color newColor = *this;

    newColor.a += A.a;
    newColor.r += A.r;
    newColor.g += A.g;
    newColor.b += A.b;
    newColor.alpha += A.alpha;
    return newColor;
  };

  color operator*(float A) { return {A * a, A * r, A * g, A * b, A * alpha}; };

  color colorGradient(color B, float per) {
    color c1;
    float invPer = 1.0f - per;

    c1.alpha = 1.0f;

    c1.a = a * invPer + B.a * per;
    c1.r = r * invPer + B.r * per;
    c1.g = g * invPer + B.g * per;
    c1.b = b * invPer + B.b * per;
    return c1;
  };

  void normalize() {
    if (alpha > 1.0f)
      alpha = 1.0f;
    if (r > 1.0f)
      r = 1.0f;
    if (g > 1.0f)
      g = 1.0f;
    if (b > 1.0f)
      b = 1.0f;
    if (a > 1.0f)
      a = 1.0f;
    if (alpha < 0.0f)
      alpha = 0.0f;
    if (r < 0.0f)
      r = 0.0f;
    if (g < 0.0f)
      g = 0.0f;
    if (b < 0.0f)
      b = 0.0f;
    if (a < 0.0f)
      a = 0.0f;
  }
};

struct pixel {
  float a = 1.0f;
  float r = 1.0f;
  float g = 1.0f;
  float b = 1.0f;
  float alpha = 1.0f;

  color convToColor() {
    color c1;
    c1.a = a;
    c1.r = r;
    c1.g = g;
    c1.b = b;
    c1.alpha = alpha;
    return c1;
  }
  void normalize() {
    if (alpha > 1.0f)
      alpha = 1.0f;
    if (r > 1.0f)
      r = 1.0f;
    if (g > 1.0f)
      g = 1.0f;
    if (b > 1.0f)
      b = 1.0f;
    if (a > 1.0f)
      a = 1.0f;
    if (alpha < 0.0f)
      alpha = 0.0f;
    if (r < 0.0f)
      r = 0.0f;
    if (g < 0.0f)
      g = 0.0f;
    if (b < 0.0f)
      b = 0.0f;
    if (a < 0.0f)
      a = 0.0f;
  }
  void increment(pixel A) {
    a += A.a;
    r += A.r;
    g += A.g;
    b += A.b;
    alpha = A.alpha;
  };
  pixel operator+(pixel A) {
    pixel newColor = *this;
    newColor.a += A.a;
    newColor.r += A.r;
    newColor.g += A.g;
    newColor.b += A.b;
    newColor.alpha += A.alpha;
    return newColor;
  };

  void print() {
    std::cout << a << ", " << r << ", " << g << ", " << b << ", " << alpha
              << "\n";
  }

  pixel operator*(float n) { return {a * n, r * n, g * n, b * n, alpha * n}; }
};

struct Xspace {
  vec2 res = {32, 32};
  // const int maxCha = 10;
  std::vector<charac> listC;
  pixel *buffer;
  float *zbuffer;
  std::vector<float> TriangleVertexData;
  void clearTriangleBuffer();
  void WriteTriangles();

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
  void DrawOptTrig(float c1x, float c1y, float c2x, float c2y, float c3x,
                   float c3y, float cr, float cg, float cb, float ca, float z1,
                   float z2, float z3);

  void drawTriangle(const vec2 cord1, const vec2 cord2, const vec2 cord3,
                    const pixel pix = {1.0f, 1.0f, 1.0f, 1.0f}, float z1 = 1.0f,
                    float z2 = 1.0f, float z3 = 1.0f);
  void DrawCircle(vec2df pos, float radius,
                  color Col = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f});

  void AddTriangleToBuffer(float c1x, float c1y, float c2x, float c2y,
                           float c3x, float c3y, float cr, float cg, float cb,
                           float ca, float z1, float z2, float z3);

  template <typename T> void fillBufferCustom(T col) {
    for (int j = 0; j < res.y; j++) {
      for (int i = 0; i < res.x; i++) {
        color c1 = col(i, j);
        setPixel({i, j}, {c1.a, c1.r, c1.g, c1.b, c1.alpha});
      }
    }
  };
};

// 💀 slaughterhouse

struct KernelFilter {
  std::vector<float> FilterWeight = {1, 0, 0, 0, 0, 0, 0, 0, 0};
  vec2 filterRes = {3, 3};
  int stridex = 1;
  int stridey = 1;
};

//

struct xframe {
  Xspace *env;
  int zindex = 0;
  float opacity = 1.0f;
  bool enableFilter = false;
  KernelFilter filter = KernelFilter();

  void SlideFilter();
  vec2 position;

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

namespace RawGraphicsHelperApi {

void FilterKernel(KernelFilter filter, pixel *buff, int H, int W);
}
