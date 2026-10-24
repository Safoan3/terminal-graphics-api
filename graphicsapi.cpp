#include "graphicsapi.h"
#include "vectr.h"
#include <algorithm>
#include <climits>
#include <cmath>
#include <cstring>
#include <ostream>
#include <sstream>
#include <unistd.h>

constexpr const static inline char *clearcode = "\033[2J\033[H";

Xspace::Xspace(vec2 r) : res(r) {
  buffer = (pixel *)malloc(sizeof(pixel) * r.x * r.y);
  zbuffer = (float *)malloc(sizeof(float) * r.x * r.y);

  for (int i = 0; i < (r.x * r.y); i++) {
    zbuffer[i] = INFINITY;
  }
};

Xspace::Xspace(int r) : Xspace({r, r}) {};

void Xspace::setCharacterDensityList() {
  listC.clear();
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
  //  write(STDOUT_FILENO, clearcode, strlen(clearcode));

  for (int j = 0; j < res.y; j++) {
    for (int i = 0; i < res.x; i++) {
      int cord = j * res.x + i;
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
  image << "\n";

  write(STDOUT_FILENO, image.str().c_str(), strlen(image.str().c_str()));

  //  std::cout << "\n";
};

void Xspace::clear() {
  for (int i = 0; i < res.x * res.y; i++) {
    buffer[i].a = 0.0f;
    buffer[i].r = 0.0f;
    buffer[i].g = 0.0f;
    buffer[i].b = 0.0f;
    buffer[i].alpha = 0.0f;

    zbuffer[i] = INFINITY;
  }
};
// pix contains alpha thats alpha for layer... this one is blending

void Xspace::setPixel(vec2 xp, pixel cac, float z, bool invert) {
  int y = xp.y;
  int x = xp.x;

  if (y >= 0 && y < res.y && x >= 0 && x < res.x) {
  } else {
    return;
  }

  if ((y * res.x) + x < res.x * res.y) {
    if (z <= zbuffer[(y * res.x) + x]) {

      pixel &l1 = buffer[(((res.y - 1) - y) * res.x) + x];

      if (invert == false) {
        l1 = buffer[y * res.x + x];
      }
      l1.a = cac.a;
      l1.r = cac.r;
      l1.g = cac.g;
      l1.b = cac.b;
      l1.alpha = cac.alpha;
      zbuffer[(y * res.x) + x] = z;
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

    if (x >= res.x || p1.y + yy >= res.y) {
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

void Xspace::DrawOptTrig(float c1x, float c1y, float c2x, float c2y, float c3x,
                         float c3y, float cr, float cg, float cb, float ca,
                         float z1, float z2, float z3) {
  int Xm = std::max<int>((int)c1x, std::max<int>((int)c2x, (int)c3x));
  int Ym = std::max<int>((int)c1y, std::max<int>((int)c2y, (int)c3y));
  int Xmin = std::min<int>((int)c1x, std::min<int>((int)c2x, (int)c3x));
  int Ymin = std::min<int>((int)c1y, std::min<int>((int)c2y, (int)c3y));

  if (Xm > res.x)
    Xm = res.x;
  if (Ym > res.y)
    Ym = res.y;
  if (Xmin < 0)
    Xmin = 0;
  if (Ymin < 0)
    Ymin = 0;

  int ys = Ym - Ymin;
  int xs = Xm - Xmin;

  for (int j = 0; j < ys; j++) {
    for (int i = 0; i < xs; i++) {
      vec2df barc = barycentric({(int)c1x, (int)c1y}, {(int)c2x, (int)c2y},
                                {(int)c3x, (int)c3y}, {i + Xmin, j + Ymin});
      float w1 = barc.x;
      float w2 = barc.y;
      float gamma = 1.0f - (w1 + w2);
      float ZBuffI = (1.0f / z2 * w1) + (1.0f / z3 * w2) + (1.0f / z1 * gamma);
      float ZBuff = 1.0f / ZBuffI;

      bool insides = false;

      if (w1 >= 0 && w2 >= 0 && w1 + w2 <= 1.0f) {
        insides = true;
      };
      vec2 pos = {i + Xmin, j + Ymin};

      if (insides) {
        //        setPixel(pos, {pix.a, w1, w2, gamma}, ZBuff); // matha noshto
        //        rainbow
        setPixel(pos, {1.0f, cr, cg, cb, ca}, ZBuff);
      };
    }
  }
}

void Xspace::drawTriangle(const vec2 cord1, const vec2 cord2, const vec2 cord3,
                          const pixel pix, float z1, float z2, float z3) {
  DrawOptTrig((float)cord1.x, (float)cord1.y, (float)cord2.x, (float)cord2.y,
              (float)cord3.x, (float)cord3.y, pix.r, pix.g, pix.b, 1.0f, z1, z2,
              z3);
  return;
};

void Xspace::DrawCircle(vec2df pox, float radius, color Col) {
  vec2df p1 = pox + vec2df{-radius, -radius};
  float horizontalSize = 2 * radius;

  for (float j = 0; j < (radius * 2); j++) {
    for (float i = 0; i < (radius * 2); i++) {
      vec2df newC = p1 + vec2df{i, j};
      float dist = (newC - pox).magnitude();
      float intensity = dist / radius;
      if (intensity > 1.0f)
        intensity = 1.0f;
      intensity = 1.0f - intensity;
      intensity = 1.0f;
      if (dist < radius) {
        setPixel({(int)newC.x, (int)newC.y},
                 {Col.a, Col.r, Col.g, Col.b, Col.alpha});
      }
    }
  }
}

// RawGraphics
void RawGraphicsHelperApi::FilterKernel(KernelFilter filter, pixel *buff, int H,
                                        int W) {
  const int Xface = filter.filterRes.x / 2;
  const int Yface = filter.filterRes.y / 2;
  const int stx = std::max(1, filter.stridex);
  const int sty = std::max(1, filter.stridey);

  float weightSum = 0.0f;
  for (float n : filter.FilterWeight)
    weightSum += n;
  if (weightSum == 0.0f)
    weightSum = 1.0f;
  const float inv = 1.0f / weightSum;

  std::vector<pixel> colReserve(W * H);

  for (int j = 0; j < H; j += sty) {
    for (int i = 0; i < W; i += stx) {
      pixel Sum = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
      int k = 0;

      for (int KernelJ = j - Yface; KernelJ <= j + Yface; KernelJ++) {
        for (int KernelI = i - Xface; KernelI <= i + Xface; KernelI++) {
          int cx = std::clamp(KernelI, 0, W - 1);
          int cy = std::clamp(KernelJ, 0, H - 1);
          const pixel &p = buff[cy * W + cx];
          float weight = filter.FilterWeight[k++] * inv;

          Sum.a += p.a * weight;
          Sum.r += p.r * weight;
          Sum.g += p.g * weight;
          Sum.b += p.b * weight;
          Sum.alpha += p.alpha * weight;
        }
      }

      Sum.normalize();
      Sum.alpha = 1.0f;

      const int yEnd = std::min(j + sty, H);
      const int xEnd = std::min(i + stx, W);
      for (int y = j; y < yEnd; y++)
        for (int x = i; x < xEnd; x++)
          colReserve[y * W + x] = Sum;
    }
  }

  for (int i = 0; i < W * H; i++)
    buff[i] = colReserve[i];
}
/// slaughter house
/// wwwwww
///

xframe::xframe(Xspace *f) {
  env = f;
  filter = KernelFilter();
};

space::space(vec2 res1) : finalrenderimg(res1) {
  res = {res1.x, res1.y};

  finalrenderimg.clear();
  finalrenderimg.setCharacterDensityList();
};

// filter

void xframe::SlideFilter() {
  RawGraphicsHelperApi::FilterKernel(filter, env->buffer, env->res.y,
                                     env->res.x);
}

/////////////////////////

void space::addlayer(xframe *fr) {
  //
  frames.push_back(fr);
};

void space::render() {
  // xframe *previousFrame;
  //

  finalrenderimg.clear();

  for (xframe *iif : frames) {
    vec2 res = iif->env->res;

    for (int j = 0; j < res.y; j++) {
      for (int i = 0; i < res.x; i++) {
        if (iif->enableFilter == true) {
          // iif->SlideFilter();
        }

        int cord = (res.y - j - 1) * res.x + i;
        pixel selected = iif->env->buffer[cord];
        float alpha = selected.alpha;
        float inverseAlpha = 1.0f - alpha;
        pixel finalPix;

        if (alpha <= 0.01f) {
          continue;
        }
        vec2 newPOs = {iif->position.x + i, iif->position.y + j};
        int index =
            (finalrenderimg.res.y - newPOs.y - 1) * finalrenderimg.res.x +
            newPOs.x;
        pixel prevPix;
        if (index >= 0 && index < finalrenderimg.res.y * finalrenderimg.res.x) {
          prevPix =
              finalrenderimg.buffer[(finalrenderimg.res.y - newPOs.y - 1) *
                                        finalrenderimg.res.x +
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
