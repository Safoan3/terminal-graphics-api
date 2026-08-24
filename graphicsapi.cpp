#include "graphicsapi.h"
#include <cmath>
#include <ostream>
#include <sstream>

Xspace::Xspace(int r) {
  res = r;
  buffer = (pixel *)malloc(sizeof(pixel) * r * r);
  zbuffer = (float *)malloc(sizeof(float) * r * r);

  for (int i = 0; i < (r * r); i++) {
    zbuffer[i] = INFINITY;
  }
};

void Xspace::setCharacterDensityList() {
  for (charac z : listC) {
    z.c = "  ";
  };

  listC.push_back({"  "}); // Or just "  "
  listC.push_back({"`'"});
  listC.push_back({"^!"});
  listC.push_back({"**"});
  listC.push_back({"76"});
  listC.push_back({"$$"});
  listC.push_back({"aa"});
  listC.push_back({"&&"});
  listC.push_back({"##"});
  listC.push_back({"%@"});
};

int Xspace::convDen(float t) {
  float mul = t * (float)listC.size();
  int cp = (int)mul;
  // std::cout << cp << "LOL \n";
  if (cp > listC.size() - 1) {
    return (listC.size() - 1);
  }
  return cp;
};

void Xspace::render() {
  std::stringstream image;

  for (int j = 0; j < res; j++) {
    for (int i = 0; i < res; i++) {
      int cord = j * res + i;
      pixel selected = buffer[cord];
      int densi = convDen(selected.a);

      if (densi <= listC.size() && densi >= 0) {
        charac d = listC[densi];

        int r = (int)(selected.r * 254 + 1);
        int g = (int)(selected.g * 254 + 1);
        int b = (int)(selected.b * 254 + 1);

        image << "\033[38;2;" << r << ";" << g << ";" << b << "m" << d.c
              << "\033[0m";

      } else {
        image << "  ";
      }
    }
    image << "\n";
  }
  std::cout << image.str();
  std::cout << "\n";
};

void Xspace::clear() {
  for (int i = 0; i < res * res; i++) {
    buffer[i].a = 0.0f;
    buffer[i].r = 0.0f;
    buffer[i].g = 0.0f;
    buffer[i].b = 0.0f;
    buffer[i].alpha = 0.0f;

    zbuffer[i] = INFINITY;
  }
};

void Xspace::setPixel(vec2 xp, pixel cac, float z, bool invert) {
  int y = xp.y;
  int x = xp.x;

  if (y >= 0 && y < res && x >= 0 && x < res) {
  } else {
    return;
  }

  if ((y * res) + x < res * res) {
    if (z <= zbuffer[(y * res) + x]) {

      pixel &l1 = buffer[(((res - 1) - y) * res) + x];

      if (invert == false) {
        l1 = buffer[y * res + x];
      }

      l1.a = cac.a;
      l1.r = cac.r;
      l1.g = cac.g;
      l1.b = cac.b;
      l1.alpha = cac.alpha;

      zbuffer[(y * res) + x] = z;
    }
  }
};

void Xspace::line(vec2 p1, vec2 p2, pixel pix, float z1, float z2) {
  bool isInf = false;
  if (p2.x - p1.x == 0) {
    isInf = true;
  }

  float slope = 0;
  if (isInf == false) {
    slope = ((float)p2.y - (float)p1.y) / ((float)p2.x - (float)p1.x);
  }
  if (isInf == true) {
    slope = 1000000;
  }

  int xOrigin = (p2.x - p1.x);

  bool isNeg = false;

  if (xOrigin < 0) {
    isNeg = true;
  }
  if (xOrigin == 0) {
    int yDist = std::abs(p2.y - p1.y);
    int sign = 1;
    if (p2.y - p1.y < 0) {
      sign = -1;
    }
    for (int i = 0; i < yDist; i++) {
      float w1 = (float)i / (float)yDist;
      float w2 = (1 - w1);

      float zz = w1 * z1 + w2 * z2;

      vec2 pos = {p1.x, (i * sign) + p1.y};

      setPixel(pos, pix, zz);
      // setPixel(pos, {pix.a, w1, w2, 1.0f}, zz);
    }
  };

  for (float i = 0; i < (float)std::abs(xOrigin); i = i + 0.05) {
    int yy;
    int x;

    float w1 = i / (float)std::abs(xOrigin);
    float w2 = (1 - w1);

    float zz = w1 * z1 + w2 * z2;

    if (isNeg) {
      yy = -i * slope;
      x = p1.x - i;
    } else {
      x = p1.x + i;
      yy = i * slope;
    }

    if (x >= res || p1.y + yy >= res) {
      break;
    }

    vec2 pos = {x, yy + p1.y};

    setPixel(pos, pix, zz);
    // setPixel(pos, {pix.a, w1, w2, 1.0f}, zz);
  }

  if (p1.x == p2.x && p1.y == p2.y) {
    setPixel(p1, {1.0f, pix.r, pix.g, pix.b}, z1);
    setPixel(p2, {1.0f, pix.r, pix.g, pix.b}, z2);
  }
};

vec2df barycentric(vec2 cord1, vec2 cord2, vec2 cord3, vec2 point) {
  vec2 d1 = cord1;
  vec2 d2 = cord2;
  vec2 d3 = cord3;
  vec2 p1 = point - cord1;

  d1 = d1 - cord1;
  d2 = d2 - cord1;
  d3 = d3 - cord1;

  float Ax = (float)d2.x;
  float Ay = (float)d2.y;

  float Bx = (float)d3.x;
  float By = (float)d3.y;

  float px = (float)p1.x;
  float py = (float)p1.y;

  float determinate = Ax * By - Bx * Ay;
  float dx = px * By - Bx * py;
  float dy = Ax * py - Ay * px;

  float w1 = (By * px - Bx * py) / (By * Ax - Bx * Ay);
  float w2 = (py - (Ay * w1)) / By;

  w1 = dx / determinate;
  w2 = dy / determinate;

  return {w1, w2};
};

bool Xspace::isInsideTriangle(vec2 cord1, vec2 cord2, vec2 cord3, vec2 point) {
  vec2df barc = barycentric(cord1, cord2, cord3, point);

  float w1 = barc.x;
  float w2 = barc.y;

  if (w1 >= 0 && w2 >= 0 && w1 + w2 <= 1.0f) {
    return true;
  };
  return false;
};

void Xspace::drawTriangle(const vec2 cord1, const vec2 cord2, const vec2 cord3,
                          const pixel pix, float z1, float z2) {
  vec2 d1 = cord1;
  vec2 d2 = cord2;
  vec2 d3 = cord3;

  int Xm = std::max<int>(d1.x, std::max<int>(d2.x, d3.x));
  int Ym = std::max<int>(d1.y, std::max<int>(d2.y, d3.y));
  int Xmin = std::min<int>(d1.x, std::min<int>(d2.x, d3.x));
  int Ymin = std::min<int>(d1.y, std::min<int>(d2.y, d3.y));

  vec2 pointA = {Xm, Ym};
  vec2 pointB = {Xm, Ymin};
  vec2 pointC = {Xmin, Ymin};
  vec2 pointD = {Xmin, Ym};

  d1 = d1 - cord1;
  d2 = d2 - cord1;
  d3 = d3 - cord1;
  int ys = Ym - Ymin;
  int xs = Xm - Xmin;

  for (int j = 0; j < ys; j++) {
    for (int i = 0; i < xs; i++) {

      vec2df barc = barycentric(cord1, cord2, cord3, {i + Xmin, j + Ymin});

      float w1 = barc.x;
      float w2 = barc.y;
      float gamma = 1.0f - (w1 + w2);
      float ZBuff = z1 * w2 + z2 * gamma;

      bool insides = false;

      if (w1 >= 0 && w2 >= 0 && w1 + w2 <= 1.0f) {
        insides = true;
      };
      vec2 pos = {i + Xmin, j + Ymin};

      // std::cout << cellZ << ", " << ZBuff;
      //
      if (insides) { //&& ZBuff < zbuffer[(res * pos.y) + pos.x]) {
        setPixel(pos, {pix.a, w1, w2, gamma}, ZBuff);
        //       setPixel(pos, pix);

        //        zbuffer[res * pos.y + pos.x] = ZBuff;

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //       render();
      };
    }
  }
};

/// slaughter house
///
///

xframe::xframe(Xspace *f) { env = f; };

space::space(vec2 res1) : finalrenderimg(res1.x) {
  res = {res1.x, res1.y};

  finalrenderimg.clear();
  finalrenderimg.setCharacterDensityList();
};

void space::addlayer(xframe *fr) {
  //
  frames.push_back(fr);
};

void space::render() {
  // xframe *previousFrame;
  //

  finalrenderimg.clear();

  for (xframe *iif : frames) {
    int res = iif->env->res;

    for (int j = 0; j < res; j++) {
      for (int i = 0; i < res; i++) {

        int cord = (res - j - 1) * res + i;
        pixel selected = iif->env->buffer[cord];
        float alpha = selected.alpha;
        float inverseAlpha = 1.0f - alpha;
        pixel finalPix;

        if (alpha <= 0.01f) {
          continue;
        }
        vec2 newPOs = {iif->positon.x + i, iif->positon.y + j};

        int index =
            (finalrenderimg.res - newPOs.y - 1) * finalrenderimg.res + newPOs.x;
        pixel prevPix;
        if (index >= 0 && index < finalrenderimg.res * finalrenderimg.res) {

          prevPix = finalrenderimg.buffer[(finalrenderimg.res - newPOs.y - 1) *
                                              finalrenderimg.res +
                                          newPOs.x];
        } else {
          std::cout << "corruption is bad if i am not involved! \n";

          continue;
        }
        finalPix.a = selected.a * alpha + prevPix.a * inverseAlpha;
        finalPix.r = selected.r * alpha + prevPix.r * inverseAlpha;
        finalPix.g = selected.g * alpha + prevPix.g * inverseAlpha;
        finalPix.b = selected.b * alpha + prevPix.b * inverseAlpha;
        finalPix.alpha = 1.0f; // Final image is fully opaque

        //  std::cout << "Pixel: " << finalPix.r << "," << finalPix.g << ","
        //          << finalPix.b << "\n";

        finalrenderimg.setPixel(newPOs, finalPix);
      };
    };
  }
}
