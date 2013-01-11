#include<cstdio>
#include<cassert>
#include<cstring>
#include<map>
#include<set>
#include<time.h>
#include<algorithm>
#include<stack>
#include<queue>
#include<utility>
#include<cmath>
#include<iostream>
#include<string>
#include<vector>
#include"lodepng.h"
using namespace std;
const double EPSILON = 1e-12;

struct vector3
{
  double x, y, z;
  vector3(double x_ = 0, double y_ = 0, double z_ = 0) : x(x_), y(y_), z(z_) {}
  vector3 operator + (const vector3& other) const {
    return vector3(x + other.x, y + other.y, z + other.z);
  }
  vector3 operator - (const vector3& other) const {
    return vector3(x - other.x, y - other.y, z - other.z);
  }
  vector3 operator * (double p) const {
    return vector3(x * p, y * p, z * p);
  }
  // dot product
  double operator * (const vector3& b) const {
    const vector3& a = *this;
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }
  // cross product
  vector3 operator ^ (const vector3& b) const {
    const vector3& a = *this;
    return vector3(a.y * b.z - a.z * b.y, 
                   a.z * b.x - a.x * b.z, 
                   a.x * b.y - a.y * b.x);
  }
  double length() const {
    return sqrt(x*x + y*y + z*z);
  }
  vector3 normalized() const {
    double l = length();
    return vector3(x / l, y / l, z / l);
  }
  // returns the projection of vector v on this
  vector3 projected(const vector3& v) const {
    vector3 n = normalized();
    return n * (n * v);
  }
};
ostream& operator << (ostream& os, const vector3& v) {
  os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
  return os;
}
vector3 operator * (double a, const vector3& b) { return b * a; }

typedef unsigned char uchar;

struct RGB {
  uchar r, g, b;
  RGB() {
    r = g = b = 0;
  }
  RGB(uchar r, uchar g, uchar b): r(r), g(g), b(b) {}
};

typedef vector<vector<RGB> > Picture;

void writePicture(const string& filename, const Picture& picture) {
  const int H = picture.size();
  const int W = picture[0].size();
  vector<unsigned char> scene(W * H * 4);
  int idx = 0;
  for(int y = 0; y < H; ++y)
    for(int x = 0; x < W; ++x) {
      scene[idx++] = picture[y][x].r;
      scene[idx++] = picture[y][x].g;
      scene[idx++] = picture[y][x].b;
      scene[idx++] = 255;
    }
  lodepng::encode(filename.c_str(), scene, W, H);
}

struct Camera {
  vector3 position;
  vector3 direction;
  vector3 up;
  vector3 right;

  Camera(vector3 p, vector3 d, vector3 u) {
    position = p;
    direction = d.normalized();
    up = u.normalized();
    up = (u - direction.projected(u)).normalized();
    right = (direction ^ up).normalized();
  }
};

void testCamera() {
  Camera c(vector3(0, 0, 0), vector3(1, 0, 0), vector3(0, 0, 1));
  cout << c.right << endl;
  assert((c.right - vector3(0, -1, 0)).length() < 1e-10);
}

struct Object {
  virtual bool getIntersection(const vector3& here, const vector3& direction, vector3& intersection) const = 0;
  virtual RGB getColor(const vector3& here, const vector3& meetAt) const = 0;
};

struct Plane: public Object {
  virtual bool getIntersection(const vector3& here, const vector3& direction, vector3& intersection) const {
    double mult = -here.z / direction.z;
    if(mult < 0) return false;
    intersection = here + direction * mult;
    return true;
  }
  virtual RGB getColor(const vector3& here, const vector3& meetAt) const {
    int offset = (int(floor(meetAt.y)) + int(floor(meetAt.x))) % 2;
    if(offset == 0) return RGB(255, 255, 255);
    return RGB(64, 64, 128);
  }
};

struct Sphere: public Object {

  vector3 center;
  double radius;
  RGB color;

  Sphere(const vector3& center, double radius, RGB color):
      center(center), radius(radius), color(color) {
  }

  // 2차방정식 ax^2 + bx + c = 0 의 모든 실근을 크기 순서대로 반환한다
  vector<double> solve2(double a, double b, double c) const
  {
    double d = b*b - 4*a*c;
    if(d < -EPSILON) return vector<double>();
    if(d < EPSILON) return vector<double>(1, -b / (2*a));
    vector<double> ret;
    ret.push_back((-b - sqrt(d)) / (2*a));
    ret.push_back((-b + sqrt(d)) / (2*a));
    return ret;
  }

  virtual bool getIntersection(const vector3& here, const vector3& direction, vector3& intersection) const {

    double a = direction * direction;
    double b = 2 * direction * (here - center);
    double c = center * center + here * here - 2 * here * center - radius * radius;
    vector<double> sols = solve2(a, b, c);
    if(sols.empty() || sols[0] < EPSILON) return false;
    intersection = here + direction * sols[0];
    return true;
  }
  virtual RGB getColor(const vector3& here, const vector3& meetAt) const {
    return color;
  }
};

struct Scene {
  vector<Object*> objects;
  vector<vector3> light_sources;

  ~Scene() {
    for(auto it: objects) 
      delete it;
  }

  void addLightSource(vector3 pos) {
    light_sources.push_back(pos);
  }

  void addObject(Object* object) {
    objects.push_back(object);
  }

  RGB cast(const vector3& here, const vector3& dir) const {
    Object* closest = nullptr;
    vector3 contact;
    RGB color;

    for(auto object: objects) {
      vector3 p;
      bool intersects = object->getIntersection(here, dir, p);
      if(!intersects) continue;

      if(closest == nullptr || (here - contact).length() > (here - p).length()) {
        closest = object;
        contact = p;
      }
    }
    if(!closest) return RGB(0, 0, 0); 

    return closest->getColor(here, contact);
  }

  Picture draw(const Camera& camera, double dist_to_camera, double size, int size_px) const {
    Picture picture(size_px, vector<RGB>(size_px));

    vector3 frustum_center = camera.position + camera.direction * dist_to_camera;
    for(int y = 0; y < size_px; ++y) {
      for(int x = 0; x < size_px; ++x) {
        double y_pos = y / double(size_px - 1) * size;
        double x_pos = x / double(size_px - 1) * size;
        vector3 mapped = frustum_center - (y_pos - 0.5) * camera.up + (x_pos - 0.5) * camera.right;
        picture[y][x] = cast(camera.position, 
                             (mapped - camera.position).normalized());
      }
    }
    return picture;
  }
};

void test() {
  testCamera();
}

int main() {
  test();
  Scene scene;
  scene.addObject(new Plane());
  scene.addObject(new Sphere(vector3(4, 4, 1), 1, RGB(192, 0, 0)));
  scene.addLightSource(vector3(1, -1, 20));
  Camera camera(vector3(0, 0, 2), vector3(2, 2, -0.5), vector3(1, 1, 0));
  Picture picture = scene.draw(camera, 1, 1, 480);
  writePicture("scene.png", picture);
}

