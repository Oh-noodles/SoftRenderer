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
    width/2., 0,          0,        width/2.,
    0,        height/2.,  0,        height/2.,
    0,        0,          255/2.,   255/2.,
    0,        0,          0,        1
});
Matrix44f Projection({
    1,  0,  0,    0,
    0,  1,  0,    0,
    0,  0,  1,    0,
    0,  0,  -0.2,  1
    });
Matrix44f MatrixCameraToWorld = Matrix44f({
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 3, 1
});
Matrix44f MatrixWorldToCamera = MatrixCameraToWorld.inverse().transpose();
Model *model;

struct SimpleShader: public IShader {
  virtual Vec4f vertex(int iface, int nthvert) {
    Vec3f v_3f = model->vert(model->face(iface)[nthvert]);
    Vec4f v = Viewport * Projection * MatrixWorldToCamera * embed<4>(v_3f);
    for (int i = 0; i < 3; i++) {
      v[i] /= v[3];
    }
    return v;
  }
  virtual bool fragment(Vec4f v0, Vec4f v1, Vec4f v2, Vec4f p, TGAColor c0, TGAColor c1, TGAColor c2, float ity0, float ity1, float ity2, TGAColor &color) {
    float area = edgeFunction(embed<2>(v0), embed<2>(v1), embed<2>(v2));
    float w0 = edgeFunction(embed<2>(v1), embed<2>(v2), embed<2>(p)) / (float)area;
    float w1 = edgeFunction(embed<2>(v2), embed<2>(v0), embed<2>(p)) / (float)area;
    float w2 = edgeFunction(embed<2>(v0), embed<2>(v1), embed<2>(p)) / (float)area;

    float z = 1.0 / ( 1.0/v0.z * w0 + 1.0/v1.z * w1 + 1.0/v2.z * w2 );

    float r = z * (c0.r/(float)v0.z * w0 + c1.r/(float)v1.z * w1 + c2.r/(float)v2.z * w2);
    float g = z * (c0.g/(float)v0.z * w0 + c1.g/(float)v1.z * w1 + c2.g/(float)v2.z * w2);
    float b = z * (c0.b/(float)v0.z * w0 + c1.b/(float)v1.z * w1 + c2.b/(float)v2.z * w2);

    float ity = z * (ity0/(float)v0.z * w0 + ity1/(float)v1.z * w1 + ity2/(float)v2.z * w2);
    ity = std::min(1.0f, ity);
    ity = std::max(0.0f, ity);
    
    color = TGAColor(r * ity, g * ity, b * ity, 255 * ity);
    return false;
  }
};


int main() {
    TGAImage image = TGAImage(width, height, TGAImage::RGB);
    model = new Model("obj/african_head.obj");
    std::cout << "nVerts: " << model->nverts() << std::endl;
    std::cout << "nFaces: " << model->nfaces() << std::endl;

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

    SimpleShader shader;

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> f = model->face(i);
        Vec3f v0, v1, v2, vt0, vt1, vt2, nt0, nt1, nt2;
        v0 = model->vert(f[0]);
        v1 = model->vert(f[1]);
        v2 = model->vert(f[2]);
        vt0 = model->textureVert(f[3]);
        vt1 = model->textureVert(f[4]);
        vt2 = model->textureVert(f[5]);
        nt0 = model->normalVert(f[6]);
        nt1 = model->normalVert(f[7]);
        nt2 = model->normalVert(f[8]);

        // Vec4f v0_4f = Viewport * Projection * MatrixWorldToCamera * embed<4>(v0);
        // Vec4f v1_4f = Viewport * Projection * MatrixWorldToCamera * embed<4>(v1);
        // Vec4f v2_4f = Viewport * Projection * MatrixWorldToCamera * embed<4>(v2);
        //
        // for (int i = 0; i < 3; i++) {
        //   v0_4f[i] /= v0_4f[3];
        //   v1_4f[i] /= v1_4f[3];
        //   v2_4f[i] /= v2_4f[3];
        // }
 
        Vec4f v0_4f = shader.vertex(i, 0);
        Vec4f v1_4f = shader.vertex(i, 1);
        Vec4f v2_4f = shader.vertex(i, 2);

        std::cout << "v0_4f: " << v0_4f << std::endl;
        TGAColor c0 = textureImage.get(vt0.x * textureWidth, (1 - vt0.y) * textureHeight);
        TGAColor c1 = textureImage.get(vt1.x * textureWidth, (1 - vt1.y) * textureHeight);
        TGAColor c2 = textureImage.get(vt2.x * textureWidth, (1 - vt2.y) * textureHeight);
        float ity0 = nt0 * light_dir;
        float ity1 = nt1 * light_dir;
        float ity2 = nt2 * light_dir;
        triangle(image, zBuffer, shader, v0_4f, v1_4f, v2_4f, c0, c1, c2, ity0, ity1, ity2);
    }
    image.flip_vertically();
    image.write_tga_file("output.tga");
    return 0;
}
