#include "tgaimage.h"
#include "geometry.h"

int project(Vec3f &v, float *matrix);
float edgeFunction(Vec2i v0, Vec2i v1, Vec2i v2);
bool inside(Vec2i v0, Vec2i v1, Vec2i v2, Vec2i p);
int line(TGAImage &image, int x1, int y1, int x2, int y2, TGAColor color);
int triangle(TGAImage &image, TGAImage &zBuffer, Vec3i v0, Vec3i v1, Vec3i v2, TGAColor c0, TGAColor c1, TGAColor c2, float ity0, float ity1, float ity2);