#pragma once
#include <cmath>
#include <iostream>

struct vec2 {
  int x;
  int y;

  vec2(int p, int l) {
    x = p;
    y = l;
  };

  vec2 operator+(int sum) { return {x + sum, y + sum}; };

  vec2 operator-(vec2 n) { return {x - n.x, y - n.y}; };

  vec2 operator+(vec2 n) { return {x + n.x, y + n.y}; };

  int dot(vec2 n) { return (n.x * x) + (n.y * y); }

  int mag() { return std::sqrt(x * x + y * y); }
  vec2() {
    x = 0;
    y = 0;
  };
};

struct vec2df {
  float x;
  float y;

  void print() { std::cout << x << ", " << y << "\n"; }

  vec2df operator*(float s) { return {x * s, y * s}; }
  vec2df operator+(float s) { return {s + x, y + s}; }
  vec2df operator+(vec2df v) { return {x + v.x, y + v.y}; }
  vec2df operator-(vec2df v) { return {x - v.x, y - v.y}; }
  float magnitude() { return std::sqrt(x * x + y * y); }
};

struct vec3df {
  float x;
  float y;
  float z;

  vec3df cross(vec3df p) {
    return {y * p.z - z * p.y, -(x * p.z - z * p.x), (x * p.y - y * p.x)};
  }
  float dot(vec3df p) { return p.x * x + p.y * y + p.z * z; };

  float magnitude() { return std::sqrt(x * x + y * y + z * z); }

  vec3df normalize() {
    float mag = magnitude();
    return {x / mag, y / mag, z / mag};
  }

  vec3df operator-(vec3df p) { return {x - p.x, y - p.y, z - p.z}; }
  vec3df operator+(vec3df p) { return {x + p.x, y + p.y, z + p.z}; }
  vec3df operator*(float s) { return {x * s, y * s, z * s}; }

  vec3df lerp(vec3df target, const float weight) {
    return ((target - vec3df{x, y, z}) * weight) + vec3df{x, y, z};
  };

  void print() { std::cout << "{" << x << ", " << y << ", " << z << "} \n"; }
};
