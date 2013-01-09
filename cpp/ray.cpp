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
int main() {
  vector<unsigned char> scene(640*480*4);
  for(int y = 0; y < 480; ++y)
    for(int x = 0; x < 640; ++x) {
      if((y / 50) % 2 == 0) {
        scene[(y*640+x)*4+1] = (y+x) % 255;
        scene[(y*640+x)*4+3] = 255;
      }
    }
  lodepng::encode("hello.png", scene, 640, 480);
}

