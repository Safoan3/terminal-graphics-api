

#include <algorithm>
#include <alloca.h>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <cstdlib>

#include "../graphicsapi.h"

#include <iostream>
#include <vector>

const float pi = 3.14159265f;
const int resolution = 32;

Xspace env(resolution);

struct color {
  float a;
  float r;
  float g;
  float b;
};

struct camera {
  vec3df position;
  vec3df direction;
  float pitch = 0.0f;
  float yaw = 0.0f;
  float roll = 0.0f;
};

vec2df perspective(vec3df a) {
  vec3df p1 = a;
  float iz = 1.0f;
  float forcalLength = 140.0f;

  if (a.z < 0.1f) {
    iz = 0.1f;
  } else {
    iz = 1.0f / a.z;
  }

  vec2df ret = {p1.x * iz * forcalLength, p1.y * iz * forcalLength};
  return ret;
};

vec2df project(const camera &cam, vec3df p1) {
  vec3df pos = cam.position;
  vec3df dir = cam.direction;
  dir.normalize();

  vec3df left = dir.cross({0, 1.0f, 0});
  left = left.normalize();

  vec3df top = left.cross(dir);

  vec3df nP1 = p1 - pos;
  float zmag = nP1.magnitude();

  vec2df newProj = perspective({nP1.dot(left), nP1.dot(top), zmag});
  vec2df screenpos = {(float)resolution / 2.0f, (float)resolution / 2.0f};

  newProj = newProj + screenpos;

  return newProj;
};

vec3df EulerCord(float x, float y) {
  return {cosf(x) * cosf(y), sinf(y), sinf(x) * cosf(y)};
}

struct object {
  std::vector<vec3df> vertices;
  std::vector<vec3df> dvertices;
  std::vector<vec3df> normals;
  std::vector<vec3df> dnormals;

  std::vector<int> edges;
  std::string name;
  color col3;
  vec3df position;
  float rotX, rotY, rotZ;
};

std::vector<object *> objs;

void init() {
  env.setCharacterDensityList();
  //  zbuffers.reserve(resolution * resolution);
};

// rendering methods //////////////////////////////////

namespace render {

vec3df lightDirX = {0.0f, -1.0f, 0.0f};
vec3df lightDir = lightDirX.normalize();

void wireframe(camera &cam) {
  env.clear();
  for (object *ob : objs) {
    object o = (*ob);

    for (int edgeIndex = 0; edgeIndex < o.edges.size();
         edgeIndex = edgeIndex + 2) {

      int edge2 = -1;
      int edge1 = o.edges[edgeIndex];

      if (edgeIndex + 1 < o.edges.size()) {
        edge2 = o.edges[edgeIndex + 1];
      }

      if (edge2 > -1 && edge2 < o.vertices.size()) {
        vec3df v1 = o.vertices[edge1] + o.position;
        vec3df v2 = o.vertices[edge2] + o.position;

        vec3df v1Rel = v1 - cam.position;
        vec3df v2Rel = v2 - cam.position;

        float dot1 = v1Rel.dot(cam.direction);
        float dot2 = v2Rel.dot(cam.direction);
        //        std::cout << "{dot1, dot2} = " << dot1 << ", " << dot2 <<
        //        "\n";
        //
        if (dot1 > 0 && dot2 > 0) {
          vec2df vp = project(cam, v1);
          vec2df vp2 = project(cam, v2);

          vp = vp;
          vp2 = vp2;

          vec2 pz = {(int)vp.x, (int)vp.y};
          vec2 py = {(int)vp2.x, (int)vp2.y};

          float shading = 0.0f;
          float ambient = 0.35f;

          if (edge1 < o.normals.size()) {
            shading = o.normals[edge1].dot(lightDir * -1);
            shading = std::clamp(shading, 0.0f, 1.0f);
          }

          float z1relcam = v1Rel.magnitude();
          float z2relcam = v2Rel.magnitude();

          shading = ambient + (1 - ambient) * shading;

          env.line(pz, py,
                   {shading, o.col3.r * shading, o.col3.g * shading,
                    o.col3.b * shading},
                   z1relcam, z2relcam);

        } else {
          // std::cout << "out of sight! \n";
        }
      }
    }
  };
  env.render();
}

} // namespace render
/// object transformation

namespace obj_transform {

vec3df rotOperation(vec3df v, float rx, float ry, float rz) {
  float xx = v.x;
  float yy = v.y;
  float zz = v.z;
  float r = std::sqrt(xx * xx + yy * yy);
  float currentAngle = atan2(yy, xx);
  float newAngle = currentAngle + rx;
  vec3df nc = {cosf(newAngle) * r, sinf(newAngle) * r, zz};
  r = std::sqrt(nc.x * nc.x + nc.z * nc.z);
  currentAngle = atan2(nc.z, nc.x);
  newAngle = currentAngle + ry;
  vec3df nc1 = {sinf(newAngle) * r, nc.y, cosf(newAngle) * r};
  r = std::sqrt(nc1.z * nc1.z + nc1.y * nc1.y);
  currentAngle = atan2(nc1.z, nc1.y);
  newAngle = currentAngle + rz;
  vec3df nc2 = {nc1.x, cosf(newAngle) * r, sinf(newAngle) * r};
  return nc2;
}

void rotate(object *obj) {
  float rx = obj->rotX;
  float ry = obj->rotY;
  float rz = obj->rotZ;

  for (int i = 0; i < obj->dvertices.size(); i++) {
    vec3df v = obj->dvertices[i];
    vec3df nc2 = rotOperation(v, rx, ry, rz);

    obj->vertices[i] = nc2;
  };
  for (int i = 0; i < obj->dnormals.size(); i++) {
    vec3df v = obj->dnormals[i];
    vec3df nc2 = rotOperation(v, rx, ry, rz);

    obj->normals[i] = nc2;
  };
}

} // namespace obj_transform

//////////////////////////////////

namespace obj_list {
object *createDonut(vec3df position, float rad, float innerrad, int iv,
                    int cv) {
  object *newObj = new object();

  newObj->position = position;
  newObj->col3 = {0.5f, 1.0f, 1.0f, 1.0f};
  newObj->name = "wet donut ~//w//~";

  // newObj.vertices.resize(iv * cv);
  // newObj.edges.resize(iv * cv);

  float incPerstep = 2 * pi / (float)cv;
  float incPerstepI = 2 * pi / (float)iv;

  std::vector<vec3df> vecx;

  for (int i = 0; i < cv; i++) {
    vec3df ccc = {cosf(incPerstep * i) * rad, 0, sinf(incPerstep * i) * rad};
    vec3df c9 = ccc.cross({0, 1, 0}).normalize();

    for (int j = 0; j < iv; j++) {
      vec3df inerC = {0, innerrad * sinf(j * incPerstepI), 0};
      vec3df tempcity = (ccc.normalize() * innerrad * cosf(j * incPerstepI));

      inerC = inerC + tempcity;
      inerC = inerC + ccc;

      vecx.push_back(inerC);
      newObj->normals.push_back(inerC.normalize());
      newObj->dnormals.push_back(inerC.normalize());

      newObj->edges.push_back((i * iv) + j);
      if (j + 1 < cv) {
        newObj->edges.push_back((i * iv) + (j + 1));
      }
      newObj->edges.push_back((i * iv) + j);
      if (i + 1 >= iv) {
        newObj->edges.push_back(((0) * iv) + j);
      } else {
        newObj->edges.push_back(((i + 1) * iv) + j);
      }
    }
    newObj->edges.push_back((i * iv));
    newObj->edges.push_back((i * iv) + (cv - 1));
  }
  newObj->vertices = vecx;
  newObj->dvertices = vecx;
  return newObj;
}
} // namespace obj_list
////////////////////////////////////////////////////
camera cam;

const float fps = 30.0f;
const int frameTime = (int)((1.0f / fps) * 1000.0f);

int main() {
  init();
  cam.position = {0, 0, 0.0f};
  cam.direction = {1, 0, 0};

  cam.position = cam.position + cam.direction * -25.0f;

  object *abbasCube = obj_list::createDonut({0, 0, 0}, 2.0f, 0.8f, 24, 24);

  objs.push_back(abbasCube);

  int totalLatency = 0;
  int currentFrame = 0;

  float step = 0;
  abbasCube->rotZ = pi / 2.0f;

  while (true) {
    abbasCube->rotX = abbasCube->rotX + 0.015 / 1.0f;
    abbasCube->rotY = abbasCube->rotY + 0.010 / 1.0f;
    abbasCube->rotZ = abbasCube->rotZ + 0.015 / 1.0f;

    abbasCube->position = {0, sinf(step * 15.0f) * 0.7f, 0};

    abbasCube->col3 = {1.0f, (sinf(step) + 1.0f) / 2.0f,
                       (cosf(step * 1.2f) + 1.0f) / 2.0f, 1.0f};

    if (step > 2 * pi) {
      step = 0;
    }

    obj_transform::rotate(abbasCube);

    std::this_thread::sleep_for(std::chrono::milliseconds(frameTime));
    //   std::cout << abbasCube.rotX << "\n";
    step = step + 0.001;
    render::wireframe(cam);
  }
  return 0;
}
