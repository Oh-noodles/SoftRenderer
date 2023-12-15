#include "tgaimage.h"
#include "geometry.h"

struct IShader {
  // virtual ~IShader();
  virtual Vec4f vertex(int iface, int nthvert) = 0;
  virtual bool fragment(Vec4f v0, Vec4f v1, Vec4f v2, Vec4f p, TGAColor c0, TGAColor c1, TGAColor c2, float ity0, float ity1, float ity2, TGAColor &color) = 0;
};

Vec3f project(Vec3f &v, Matrix44f matrix);
float edgeFunction(Vec2i v0, Vec2i v1, Vec2i v2);
float edgeFunction(Vec2f v0, Vec2f v1, Vec2f v2);
bool inside(Vec2f v0, Vec2f v1, Vec2f v2, Vec2f p);
int line(TGAImage &image, int x1, int y1, int x2, int y2, TGAColor color);
int triangle(TGAImage &image, TGAImage &zBuffer, IShader &shader, Vec4f v0, Vec4f v1, Vec4f v2, TGAColor c0, TGAColor c1, TGAColor c2, float ity0, float ity1, float ity2);
