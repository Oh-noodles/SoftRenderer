#include <vector>
#include <cmath>
#include <stdlib.h>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

#define INF 99999999
Vec3f light_dir(0,0,-1); // define light_dir

const int width = 800;
const int height = 800;
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor white = TGAColor(255, 255, 255, 255);
float zBuffer[width][height] = {};


int initZBuffer() {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            zBuffer[i][j] = INF;
        }
    }
    return 0;
}

int project(Vec3f &v, float *matrix) {
    v = productMatrix(v, matrix);
    v.x /= -v.z;
    v.y /= -v.z;
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

int triangle(TGAImage &image, Vec3i v0, Vec3i v1, Vec3i v2, TGAColor c0, TGAColor c1, TGAColor c2) {
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
                if (z < -1 && abs(z) < zBuffer[x][y]) {
                    // c * 1/z = c0 * 1/z0 * w0 + c1 * 1/z1 * w1 + c2 * 1/z2 * w2
                    // c = z * (c0 * 1/z0 * w0 + c1 * 1/z1 * w1 + c2 * 1/z2 * w2)
                    float r = z * (c0.r/(float)v0.z * w0 + c1.r/(float)v1.z * w1 + c2.r/(float)v2.z * w2);
                    float g = z * (c0.g/(float)v0.z * w0 + c1.g/(float)v1.z * w1 + c2.g/(float)v2.z * w2);
                    float b = z * (c0.b/(float)v0.z * w0 + c1.b/(float)v1.z * w1 + c2.b/(float)v2.z * w2);

                    zBuffer[x][y] = abs(z);
                    image.set(x, y, TGAColor(r, g, b, 255));
                }
            }
        }
    }

    return 0;
}

int main() {
    initZBuffer();

    TGAImage image = TGAImage(width, height, TGAImage::RGB);
    Model model("obj/african_head.obj");
    std::cout << "nVerts: " << model.nverts() << std::endl;
    std::cout << "nFaces: " << model.nfaces() << std::endl;

    TGAImage textureImage = TGAImage();
    textureImage.read_tga_file("african_head_diffuse.tga");
    const int textureWidth = textureImage.get_width();
    const int textureHeight = textureImage.get_height();
    std::cout << "textureWidth: " << textureWidth << ", textureHeight: " << textureHeight << std::endl;

    float MatrixCameraToWorld[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 2, 1
    };
    float MatrixWorldToCamera[16];
    inverseMatrix(MatrixCameraToWorld, MatrixWorldToCamera);

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> f = model.face(i);
        Vec3f v0, v1, v2, vt0, vt1, vt2;
        v0 = model.vert(f[0]);
        v1 = model.vert(f[1]);
        v2 = model.vert(f[2]);
        vt0 = model.textureVert(f[3]);
        vt1 = model.textureVert(f[4]);
        vt2 = model.textureVert(f[5]);
        project(v0, MatrixWorldToCamera);
        project(v1, MatrixWorldToCamera);
        project(v2, MatrixWorldToCamera);

        Vec3f world_coords[3] = {v0, v1, v2};
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
        n.normalize(); 
        float intensity = n*light_dir; 
        if (intensity > 0) {
            Vec3i v0_3i = Vec3i((v0.x + 1) / 2.0 * width, (v0.y + 1) / 2.0 * height, (v0.z + 1) / 2.0 * width);
            Vec3i v1_3i = Vec3i((v1.x + 1) / 2.0 * width, (v1.y + 1) / 2.0 * height, (v1.z + 1) / 2.0 * width);
            Vec3i v2_3i = Vec3i((v2.x + 1) / 2.0 * width, (v2.y + 1) / 2.0 * height, (v2.z + 1) / 2.0 * width);
            TGAColor c0 = textureImage.get(vt0.x * textureWidth, (1 - vt0.y) * textureHeight);
            TGAColor c1 = textureImage.get(vt1.x * textureWidth, (1 - vt1.y) * textureHeight);
            TGAColor c2 = textureImage.get(vt2.x * textureWidth, (1 - vt2.y) * textureHeight);
            triangle(image, v0_3i, v1_3i, v2_3i, c0, c1, c2);
        }
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}