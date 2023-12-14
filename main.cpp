#include <vector>
#include <cmath>
#include <stdlib.h>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "gl.h"
#include <iostream>

#define INF 99999999
Vec3f light_dir(1,-1,1); // define light_dir

const int width = 800;
const int height = 800;
Matrix44f Viewport = Matrix44f({
    width,  0,      0,      0,
    0,      height, 0,      0,
    0,      0,      255,    0,
    0,      0,      0,      1
});
Matrix44f Projection({
    1,  0,  0,    0,
    0,  1,  0,    0,
    0,  0,  1,    0,
    0,  0,  -0.5, 1
    });
Matrix44f MatrixCameraToWorld = Matrix44f({
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 3, 1
});
Matrix44f MatrixWorldToCamera = MatrixCameraToWorld.inverse();

int main() {
    TGAImage image = TGAImage(width, height, TGAImage::RGB);
    Model model("obj/african_head.obj");
    std::cout << "nVerts: " << model.nverts() << std::endl;
    std::cout << "nFaces: " << model.nfaces() << std::endl;

    TGAImage zBuffer = TGAImage(image.get_width(), image.get_height(), TGAImage::GRAYSCALE);
    for (int i = 0; i < image.get_width(); i++) {
        for (int j = 0; j < image.get_height(); j++) {
            zBuffer.set(i, j, TGAColor(255, 1));
        }
    }

    TGAImage textureImage = TGAImage();
    textureImage.read_tga_file("texture/african_head_diffuse.tga");
    const int textureWidth = textureImage.get_width();
    const int textureHeight = textureImage.get_height();
    std::cout << "textureWidth: " << textureWidth << ", textureHeight: " << textureHeight << std::endl;

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<int> f = model.face(i);
        Vec3f v0, v1, v2, vt0, vt1, vt2, nt0, nt1, nt2;
        v0 = model.vert(f[0]);
        v1 = model.vert(f[1]);
        v2 = model.vert(f[2]);
        vt0 = model.textureVert(f[3]);
        vt1 = model.textureVert(f[4]);
        vt2 = model.textureVert(f[5]);
        nt0 = model.normalVert(f[6]);
        nt1 = model.normalVert(f[7]);
        nt2 = model.normalVert(f[8]);

        // Vec4f v0_4f = Projection * MatrixWorldToCamera * embed<4>(v0);
        // Vec4f v1_4f = Projection * MatrixWorldToCamera * embed<4>(v1);
        // Vec4f v2_4f = Projection * MatrixWorldToCamera * embed<4>(v2);
        Vec4f v0_4f = Projection * embed<4>(v0) * MatrixWorldToCamera;
        Vec4f v1_4f = Projection * embed<4>(v1) * MatrixWorldToCamera;
        Vec4f v2_4f = Projection * embed<4>(v2) * MatrixWorldToCamera;
        std::cout << "v0_4f: " << v0_4f << std::endl;
        
        for (int i = 0; i < 3; i++) {
          v0_4f[i] /= v0_4f[3];
          v1_4f[i] /= v1_4f[3];
          v2_4f[i] /= v2_4f[3];
        }
        v0_4f.x = (v0_4f.x + 1) / 2.;
        v0_4f.y = (v0_4f.y + 1) / 2.;
        v0_4f.z = (v0_4f.z + 1) / 2.;
        v1_4f.x = (v1_4f.x + 1) / 2.;
        v1_4f.y = (v1_4f.y + 1) / 2.;
        v1_4f.z = (v1_4f.z + 1) / 2.;
        v2_4f.x = (v2_4f.x + 1) / 2.;
        v2_4f.y = (v2_4f.y + 1) / 2.;
        v2_4f.z = (v2_4f.z + 1) / 2.;
        
        

        v0_4f = Viewport * v0_4f;
        v1_4f = Viewport * v1_4f;
        v2_4f = Viewport * v2_4f;
        

        v0 = Viewport * project(v0, MatrixWorldToCamera);
        v1 = Viewport * project(v1, MatrixWorldToCamera);
        v2 = Viewport * project(v2, MatrixWorldToCamera);
        TGAColor c0 = textureImage.get(vt0.x * textureWidth, (1 - vt0.y) * textureHeight);
        TGAColor c1 = textureImage.get(vt1.x * textureWidth, (1 - vt1.y) * textureHeight);
        TGAColor c2 = textureImage.get(vt2.x * textureWidth, (1 - vt2.y) * textureHeight);
        float ity0 = nt0 * light_dir;
        float ity1 = nt1 * light_dir;
        float ity2 = nt2 * light_dir;
        // triangle(image, zBuffer, embed<4>(v0), embed<4>(v1), embed<4>(v2), c0, c1, c2, ity0, ity1, ity2);
        triangle(image, zBuffer, v0_4f, v1_4f, v2_4f, c0, c1, c2, ity0, ity1, ity2);
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}
