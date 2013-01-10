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
    right = (direction ^ up).normalized();
  }
};

void testCamera() {
  Camera c(vector3(0, 0, 0), vector3(1, 0, 0), vector3(0, 0, 1));
  cout << c.right << endl;
  assert((c.right - vector3(0, -1, 0)).length() < 1e-10);
}

struct Object {
  virtual vector3 meet(const vector3& here, const vector3& direction) const = 0;
  virtual RGB reflect(const vector3& here, const vector3& meetAt) const = 0;
};

struct Scene {
  RGB cast(const vector3& here, const vector3& dir) const {
    return RGB(0, 0, 255);
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
  Camera camera(vector3(0, 0, 10), vector3(0, 0, -1), vector3(0, 1, 0));
  Picture picture = scene.draw(camera, 1, 3, 480);
  writePicture("scene.png", picture);
}

