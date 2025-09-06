// Based on Dmitrii Burkov's Vec lib

#ifndef VEC_H
#define VEC_H

#include <cmath>

template <typename T> T clamp(T a, T min, T max) { if (a > max) { return max; } else if (a < min) { return min; } return a; }

#define VEC_FOR for(size_t i = 0; i < N; i++)
#define VEC_TEMPLATE template <typename T, size_t N>
#define VEC_TEMPLATE_T template <typename T>
#define VEC_TEMPLATE_N template <size_t N>
#define VEC Vec<T, N>
#define VEC_3 Vec<T, 3>

#define VEC_DEFAULTS(N) \
	static const size_t n = N;\
	Vec() { for(size_t i = 0; i < N; i++) ptr[i] = T(0); }\
	Vec(const T& a) { for(int i = 0; i < N; i++) ptr[i] = a; }\
	template <typename T2> Vec(const Vec<T2, N>& v) { for(int i = 0; i < N; i++) ptr[i] = (T)v[i]; }\
	T& operator[] (size_t i) { return ptr[i]; }\
	const T& operator[] (size_t i) const { return ptr[i]; }\
	T lengthSquared() const { T r = (T)0; for(int i = 0; i < N; i++) r += ptr[i]*ptr[i]; return r; }\
    T length() const { return std::sqrt(lengthSquared()); }\
	T sum() const { T r = (T)0; for(int i = 0; i < N; i++) r += ptr[i]; return r; }\
    static const Vec<T, N> ZERO;\
    static const Vec<T, N> IDENTITY;

VEC_TEMPLATE struct Vec;

VEC_TEMPLATE_T struct Vec<T, 3> {
	union {
		struct { T x; T y; T z; };
		T ptr[3];
	};

	VEC_DEFAULTS(3);
	Vec(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}
	T lenXYSquared() const { T r = ptr[0] * ptr[0] + ptr[1] * ptr[1]; return r; }
	T lenXY() const { return std::sqrt(lenXYSquared()); }
};

// Constants
VEC_TEMPLATE_T const VEC_3 VEC_3::ZERO = VEC_3((T)0, (T)0, (T)0);
VEC_TEMPLATE_T const VEC_3 VEC_3::IDENTITY = VEC_3((T)0, (T)0, (T)1);

// Negate operator
VEC_TEMPLATE VEC  operator - (const VEC& v1) { VEC v; VEC_FOR v[i] = -v1[i]; return v; }

// Vector-scalar math operators
VEC_TEMPLATE VEC& operator += (VEC& v1, const T& a) { VEC_FOR v1[i] += a; return v1; }
VEC_TEMPLATE VEC& operator -= (VEC& v1, const T& a) { VEC_FOR v1[i] -= a; return v1; }
VEC_TEMPLATE VEC& operator *= (VEC& v1, const T& a) { VEC_FOR v1[i] *= a; return v1; }
VEC_TEMPLATE VEC& operator /= (VEC& v1, const T& a) { VEC_FOR v1[i] /= a; return v1; }
VEC_TEMPLATE VEC  operator +  (const VEC& v1, const T& a) { VEC v; VEC_FOR v[i] = v1[i] + a; return v; }
VEC_TEMPLATE VEC  operator -  (const VEC& v1, const T& a) { VEC v; VEC_FOR v[i] = v1[i] - a; return v; }
VEC_TEMPLATE VEC  operator *  (const VEC& v1, const T& a) { VEC v; VEC_FOR v[i] = v1[i] * a; return v; }
VEC_TEMPLATE VEC  operator /  (const VEC& v1, const T& a) { VEC v; VEC_FOR v[i] = v1[i] / a; return v; }

// Vector-vector math operators
VEC_TEMPLATE VEC  operator +  (const VEC& v1, const VEC& v2) { VEC v; VEC_FOR v[i] = v1[i] + v2[i]; return v; }
VEC_TEMPLATE VEC  operator -  (const VEC& v1, const VEC& v2) { VEC v; VEC_FOR v[i] = v1[i] - v2[i]; return v; }
VEC_TEMPLATE VEC operator * (const VEC& v1, const VEC& v2) { VEC v; VEC_FOR v[i] = v1[i] * v2[i]; return v; }
VEC_TEMPLATE VEC  operator /  (const VEC& v1, const VEC& v2) { VEC v; VEC_FOR v[i] = v1[i] / v2[i]; return v; }
VEC_TEMPLATE VEC& operator += (VEC& v1, const VEC& v2) { VEC_FOR v1[i] += v2[i]; return v1; }
VEC_TEMPLATE VEC& operator -= (VEC& v1, const VEC& v2) { VEC_FOR v1[i] -= v2[i]; return v1; }
VEC_TEMPLATE VEC& operator *= (VEC& v1, const VEC& v2) { VEC_FOR v1[i] *= v2[i]; return v1; }
VEC_TEMPLATE VEC& operator /= (VEC& v1, const VEC& v2) { VEC_FOR v1[i] /= v2[i]; return v1; }

// Vector algebra
VEC_TEMPLATE T dot(const VEC& v1, const VEC& v2) { VEC v; VEC_FOR v[i] = v1[i] * v2[i]; return v.sum(); }
VEC_TEMPLATE_T VEC_3 cross(const VEC_3& v1, const VEC_3& v2) { return VEC_3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }

#undef VEC_FOR
#undef VEC_TEMPLATE
#undef VEC_TEMPLATE_T
#undef VEC_TEMPLATE_N
#undef VEC
#undef VEC_3
#undef VEC_DEFAULTS


typedef Vec<int, 3> Vec3I;
typedef Vec<double, 3> Vec3;

#endif // !VEC_H
