#include "geometry.h"

Matrix44f::Matrix44f() {

}

Matrix44f::Matrix44f(std::array<float, 16> m) {
    for (int i = 0; i < 16; i++) {
        this->m[i] = m[i];
    }
}

Matrix44f::Matrix44f(float m[16]) {
    for (int i = 0; i < 16; i++) {
        this->m[i] = m[i];
    }
}

Matrix44f::~Matrix44f() {}

Matrix44f Matrix44f::inverse() {
    Matrix44f mat;
    float inv[16];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            inv[j*4+i] = invf(i, j, this->m);
        }
    }
    double D = 0;
    for (int k = 0; k < 4; k++) {
        D += this->m[k] * inv[k * 4];
    }
    if (D == 0) {
        std::cout << "D == 0" << std::endl;
        return mat;
    }
    D = 1.0 / D;
    for (int i = 0; i < 16; i++) {
        mat.m[i] = inv[i] * D;
    }
    return mat;
}

Matrix44f operator*(Matrix44f mat1, Matrix44f mat2) {
    Matrix44f mat;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            // ith row * jth column
            mat.m[i * 4 +j] = mat1.m[i * 4 + 0] * mat2.m[0 * 4 + j] + mat1.m[i * 4 + 1] * mat2.m[1 * 4 + j] + mat1.m[i * 4 + 2] * mat2.m[2 * 4 + j] + mat1.m[i * 4 + 3] * mat2.m[3 * 4 + j];
        }
    }
    return mat;
}

Vec3f operator*(Matrix44f mat, Vec3f v) {
    const int w = 4;
    return Vec3f(
        v.x * mat.m[0*w+0] + v.y * mat.m[1*w+0] + v.z * mat.m[2*w+0] + mat.m[3*w+0],
        v.x * mat.m[0*w+1] + v.y * mat.m[1*w+1] + v.z * mat.m[2*w+1] + mat.m[3*w+1],
        v.x * mat.m[0*w+2] + v.y * mat.m[1*w+2] + v.z * mat.m[2*w+2] + mat.m[3*w+2]
    );
}

float invf(int i,int j,const float* m){

    int o = 2+(j-i);

    i += 4+o;
    j += 4-o;

    #define e(a,b) m[ ((j+b)%4)*4 + ((i+a)%4) ]

    float inv =
     + e(+1,-1)*e(+0,+0)*e(-1,+1)
     + e(+1,+1)*e(+0,-1)*e(-1,+0)
     + e(-1,-1)*e(+1,+0)*e(+0,+1)
     - e(-1,-1)*e(+0,+0)*e(+1,+1)
     - e(-1,+1)*e(+0,-1)*e(+1,+0)
     - e(+1,-1)*e(-1,+0)*e(+0,+1);

    return (o%2)?inv : -inv;

    #undef e

}