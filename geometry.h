#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <array>
#include <cassert>
#include <iostream>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <size_t DIM, typename T>
struct Vec {
	private:
		T _data[DIM];
	public:
		Vec() {
			for (size_t i = 0; i < DIM; i++) {
				_data[i] = T();
			}
		}
		T& operator[](size_t i) {
			assert(i < DIM);
			return _data[i];
		}
};

template <typename T>
struct Vec<2, T> {
	T x, y;
	Vec() {
		x = T();
		y = T();
	}
	Vec(T _x, T _y) {
		x = _x;
		y = _y;
	}
	T& operator[](size_t i) {
		assert(i < 2);
		return i <= 0 ? x : y;
	}
};

template <typename T>
struct Vec<3, T> {
	T x, y, z;
	Vec() {
		x = T();
		y = T();
		z = T();
	}
	Vec(T _x, T _y, T _z) {
		x = _x;
		y = _y;
		z = _z;
	}
	T& operator[](size_t i) {
		assert(i < 3);
		return i <= 0 ? x : (i == 1 ? y : z);
	}
};

template <typename T>
struct Vec<4, T> {
  T x, y, z, w;
  Vec(): x(T()), y(T()), z(T()), w(T()) {}
  Vec(T _x, T _y, T _z, T _w): x(_x), y(_y), z(_z), w(_w) {}
  T& operator[](size_t i) {
    assert(i < 4);
    return i <= 0 ? x : (i == 1 ? y : (i == 2 ? z : w));
  }
};

template <size_t LEN, size_t DIM, typename T>
Vec<LEN, T> embed(Vec<DIM, T> v, T fill = 1) {
	Vec<LEN, T> ret;
	for (size_t i = 0; i < LEN; i++) {
		ret[i] = i < DIM ? v[i] : fill;
	}
	return ret;
};

template <size_t DIM, typename T>
T operator*(Vec<DIM, T> a, Vec<DIM, T> b) {
	T ret = T();
	for (size_t i = 0; i < DIM; i++) {
		ret += a[i]*b[i];
	}
	return ret;
};

typedef Vec<2, float> Vec2f;
typedef Vec<2, int>		Vec2i;
typedef Vec<3, float>	Vec3f;
typedef Vec<3, int>		Vec3i;
typedef Vec<4, float> Vec4f;
typedef Vec<4, int>   Vec4i;

template <size_t DIM, typename T>
std::ostream& operator<<(std::ostream &s, Vec<DIM, T> &v) {
  s << "( " << v[0];
  for (int i = 1; i < DIM; i++) {
    s << ", " << v[i];
  }
  s << " )\n";
  return s;
}


class Matrix44f {
	private:
		
	public:
		float m[16] = {0};
		Matrix44f();
		Matrix44f(std::array<float, 16> m);
		Matrix44f(float m[16]);
		~Matrix44f();
		Matrix44f inverse();
    Matrix44f transpose();
};

Matrix44f operator*(Matrix44f mat1, Matrix44f mat2);
Vec3f operator*(Matrix44f mat, Vec3f v);
Vec4f operator*(Matrix44f mat, Vec4f v);
Vec4f operator*(Vec4f v, Matrix44f mat);
float invf(int i,int j,const float* m);

#endif //__GEOMETRY_H__
