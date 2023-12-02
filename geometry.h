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

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
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

// typedef Vec2<float> Vec2f;
// typedef Vec2<int>   Vec2i;
// typedef Vec3<float> Vec3f;
// typedef Vec3<int>   Vec3i;

typedef Vec<2, float> 	Vec2f;
typedef Vec<2, int>		Vec2i;
typedef Vec<3, float>	Vec3f;
typedef Vec<3, int>		Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
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
};

Matrix44f operator*(Matrix44f mat1, Matrix44f mat2);
Vec3f operator*(Matrix44f mat, Vec3f v);
float invf(int i,int j,const float* m);

#endif //__GEOMETRY_H__
