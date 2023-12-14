#include "gl.h"

Vec3f project(Vec3f &v, Matrix44f matrix) {
    Vec3f ret = matrix * v;
    ret.x /= -ret.z;
    ret.y /= -ret.z;
    ret.x = (ret.x + 1) / 2.0;
    ret.y = (ret.y + 1) / 2.0;
    ret.z = (ret.z + 1) / 2.0;
    return ret;
}

float edgeFunction(Vec2i v0, Vec2i v1, Vec2i v2) {
    return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
}

bool inside(Vec2i v0, Vec2i v1, Vec2i v2, Vec2i p) {
    float w0 = edgeFunction(v1, v2, p);
    float w1 = edgeFunction(v2, v0, p);
    float w2 = edgeFunction(v0, v1, p);
    return (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
}

int line(TGAImage &image, int x1, int y1, int x2, int y2, TGAColor color) {
    bool swaped = false;
    if (abs(x1 - x2) < abs(y1 - y2)) {
        swaped = true;
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    if (x2 < x1) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    if (x1 == x2) return 0;

    // 斜率
    float k = y2 - y1;
    // 当前 y 坐标值与实际 y 坐标值的偏差
    float dy = 0;
    float y = y1;
    for (int x = x1; x <= x2; x++) {
        dy += k;
        // 当超过 1 单位的偏差
        while (abs(dy) >= x2 - x1) {
            int diff = dy > 0 ? 1 : -1;
            y += diff;
            dy -= diff * (x2 - x1);
        }
        if (swaped) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
    return 0;
}

int triangle(TGAImage &image, TGAImage &zBuffer, Vec4f v0, Vec4f v1, Vec4f v2, TGAColor c0, TGAColor c1, TGAColor c2, float ity0, float ity1, float ity2) {
    // loop pixels in rectangle boundary
    // and use edge function to check if it is in triangle
    int minX = std::min(v0.x, std::min(v1.x, v2.x));
    int maxX = std::max(v0.x, std::max(v1.x, v2.x));
    int minY = std::min(v0.y, std::min(v1.y, v2.y));
    int maxY = std::max(v0.y, std::max(v1.y, v2.y));

    Vec2i v0_2i = Vec2i(v0.x, v0.y);
    Vec2i v1_2i = Vec2i(v1.x, v1.y);
    Vec2i v2_2i = Vec2i(v2.x, v2.y);
    float area = edgeFunction(v0_2i, v1_2i, v2_2i);
 
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (inside(Vec2i(v0.x, v0.y), Vec2i(v1.x, v1.y), Vec2i(v2.x, v2.y), Vec2i(x, y))) {
                float w0 = edgeFunction(v1_2i, v2_2i, Vec2i(x, y)) / (float)area;
                float w1 = edgeFunction(v2_2i, v0_2i, Vec2i(x, y)) / (float)area;
                float w2 = edgeFunction(v0_2i, v1_2i, Vec2i(x, y)) / (float)area;

                // 1/z = 1/z0 * k0 + 1/z1 * k1 + 1/z2 * k2
                // z = 1 / ( 1/z0 * k0 + 1/z1 * k1 + 1/z2 * k2 )
                float z = 1.0 / ( 1.0/v0.z * w0 + 1.0/v1.z * w1 + 1.0/v2.z * w2 );
                if (z < -1 && abs(z) < zBuffer.get(x, y).val) {
                    zBuffer.set(x, y, TGAColor(abs(z), 1));
                    // c * 1/z = c0 * 1/z0 * w0 + c1 * 1/z1 * w1 + c2 * 1/z2 * w2
                    // c = z * (c0 * 1/z0 * w0 + c1 * 1/z1 * w1 + c2 * 1/z2 * w2)
                    float r = z * (c0.r/(float)v0.z * w0 + c1.r/(float)v1.z * w1 + c2.r/(float)v2.z * w2);
                    float g = z * (c0.g/(float)v0.z * w0 + c1.g/(float)v1.z * w1 + c2.g/(float)v2.z * w2);
                    float b = z * (c0.b/(float)v0.z * w0 + c1.b/(float)v1.z * w1 + c2.b/(float)v2.z * w2);

                    float ity = z * (ity0/(float)v0.z * w0 + ity1/(float)v1.z * w1 + ity2/(float)v2.z * w2);
                    ity = std::min(1.0f, ity);
                    ity = std::max(0.0f, ity);

                    image.set(x, y, TGAColor(r * ity, g * ity, b * ity, 255 * ity));
                }
            }
        }
    }

    return 0;
}
