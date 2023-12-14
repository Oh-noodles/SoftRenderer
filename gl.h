#include "tgaimage.h"
#include "geometry.h"

Vec3f project(Vec3f &v, Matrix44f matrix);
float edgeFunction(Vec2i v0, Vec2i v1, Vec2i v2);
bool inside(Vec2i v0, Vec2i v1, Vec2i v2, Vec2i p);
int line(TGAImage &image, int x1, int y1, int x2, int y2, TGAColor color);
int triangle(TGAImage &image, TGAImage &zBuffer, Vec4f v0, Vec4f v1, Vec4f v2, TGAColor c0, TGAColor c1, TGAColor c2, float ity0, float ity1, float ity2);
