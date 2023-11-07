#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

#define INF 9999999

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model *model = NULL;
const int width  = 800;
const int height = 800;
Vec3f light_dir(0,0,-1); // define light_dir

float zBuffer[width][height] = {};
TGAImage textureImage;
int textureImgWidth;
int textureImgHeight;

void line(Vec2i t0, Vec2i t1, TGAImage &image, TGAColor color) {
    int x0 = t0.x, y0 = t0.y;
    int x1 = t1.x, y1 = t1.y;
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x=x0; x<=x1; x++) {
        float t = (x-x0)/(float)(x1-x0);
        // y0 - y0 * t + y1 * t;
        // y0 + (y1 - y0) * t;
        int y = y0*(1.-t) + y1*t;
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}

float edgeFunction(Vec2i v0, Vec2i v1, Vec2i v2) {
    return (v2.x - v0.x) * (v1.y - v0.y) - (v2.y - v0.y) * (v1.x - v0.x);
}

bool inside(Vec2i v0, Vec2i v1, Vec2i v2, Vec2i p) {
    float e0 = edgeFunction(v1, v2, p);
    float e1 = edgeFunction(v2, v0, p);
    float e2 = edgeFunction(v0, v1, p);
    return (e0 >= 0 && e1 >= 0 && e2 >= 0) || (e0 <= 0 && e1 <= 0 && e2 <= 0);
}

Vec2i Vec3ito2i(Vec3i v) {
    return Vec2i(v.x, v.y);
}

// redner 2D triangle
void triangle(Vec3i v0, Vec3i v1, Vec3i v2, Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image) {
    // boundary box
    int minX = std::min(v0.x, std::min(v1.x, v2.x));
    int maxX = std::max(v0.x, std::max(v1.x, v2.x));
    int minY = std::min(v0.y, std::min(v1.y, v2.y));
    int maxY = std::max(v0.y, std::max(v1.y, v2.y));

    // loop pixels in boundary box
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            Vec2i v0_2i = Vec3ito2i(v0), v1_2i = Vec3ito2i(v1), v2_2i = Vec3ito2i(v2);
            Vec2i p_2i = Vec2i(x, y);
            if (inside(v0_2i, v1_2i, v2_2i, p_2i)) {

                float area = edgeFunction(v0_2i, v1_2i, v2_2i);
                float w0 = edgeFunction(v1_2i, v2_2i, p_2i) / (float)area;
                float w1 = edgeFunction(v2_2i, v0_2i, p_2i) / (float)area;
                float w2 = edgeFunction(v0_2i, v1_2i, p_2i) / (float)area;

                // 1/z = 1/z0 * w0 + 1/z1 * w1 + 1/z2 * w2
                float z = 1.0 / ( 1.0/v0.z * w0 + 1.0/v1.z * w1 + 1.0/v2.z * w2 );

                TGAColor c0 = textureImage.get(t0.x, textureImgHeight - t0.y);
                TGAColor c1 = textureImage.get(t1.x, textureImgHeight - t1.y);
                TGAColor c2 = textureImage.get(t2.x, textureImgHeight - t2.y);

                // r * 1/z = r0 * 1/z0 * w0 + r1 * 1/z1 * w1 + r2 * 1/z2 * w2
                float r = (c0.r / (float)v0.z * w0 + c1.r / (float)v1.z * w1 + c2.r / (float)v2.z * w2) * z;
                float g = (c0.g / (float)v0.z * w0 + c1.g / (float)v1.z * w1 + c2.g / (float)v2.z * w2) * z;
                float b = (c0.b / (float)v0.z * w0 + c1.b / (float)v1.z * w1 + c2.b / (float)v2.z * w2) * z;
                TGAColor finalColor = TGAColor(r, g, b, 255);

                if (z < -1 && abs(z) < zBuffer[x][y]) {
                    image.set(x, y, finalColor);
                    zBuffer[x][y] = abs(z);
                }
            }
        }
    }
}

Vec3f project(Vec3f v, float matrix[16]) {
    v = productMatrix(v, matrix);
    v.x /= -v.z;
    v.y /= -v.z;
    return v;
}

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("obj/african_head.obj");
    }

    float matrixCamera2World[16] = {
        1,  0,  0,  0,
        0,  1,  0,  0,
        0,  0,  1,  0,
        0,  0,  1.6,  1
    };
    float matrixWorld2Camera[16] = {};
    inverseMatrix(matrixCamera2World, matrixWorld2Camera);

    // init z-buffer
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            zBuffer[i][j] = INF;
        }
    }

    TGAImage image(width, height, TGAImage::RGB);
    textureImage.read_tga_file("african_head_diffuse.tga");
    textureImgWidth = textureImage.get_width();
    textureImgHeight = textureImage.get_height();
    std::cout << "textureImgWidth: " << textureImgWidth << std::endl;
    std::cout << "textureImgHeight " << textureImgHeight << std::endl;

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f v0 = model->vert(face[0]);
        Vec3f v1 = model->vert(face[1]);
        Vec3f v2 = model->vert(face[2]);

        v0 = project(v0, matrixWorld2Camera);
        v1 = project(v1, matrixWorld2Camera);
        v2 = project(v2, matrixWorld2Camera);

        Vec3f t0 = model->textureVert(face[3]);
        Vec3f t1 = model->textureVert(face[4]);
        Vec3f t2 = model->textureVert(face[5]);

        Vec3f world_coords[3] = {v0, v1, v2};
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
        n.normalize(); 
        float intensity = n*light_dir; 
        if (intensity > 0) {
            triangle(Vec3i((v0.x+1.)*width/2., (v0.y+1.)*height/2., (v0.z+1.)*width/2), Vec3i((v1.x+1.)*width/2., (v1.y+1.)*height/2., (v1.z+1.)*width/2), Vec3i((v2.x+1.)*width/2., (v2.y+1.)*height/2., (v2.z+1.)*width/2), Vec3i(t0.x * textureImgWidth, t0.y * textureImgHeight, 0), Vec3i(t1.x * textureImgWidth, t1.y * textureImgHeight, 0), Vec3i(t2.x * textureImgWidth, t2.y * textureImgHeight, 0), image);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
    return 0;
}

