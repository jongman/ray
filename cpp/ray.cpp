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

int main() {
  Picture scene(480, vector<RGB>(640));
  for(int y = 0; y < 480; ++y)
    for(int x = 0; x < 640; ++x) {
      if((y / 50) % 2 == 0) {
        scene[y][x].r = (y+x) % 255;
      }
    }
  writePicture("scene.png", scene);
}

