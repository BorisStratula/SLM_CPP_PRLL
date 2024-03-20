#ifndef VEC3_H
#define VEC3_H

#include "intvec3.h"

class Vec3 {
public:
	double x;
	double y;
	double z;

	Vec3();
	Vec3(double x, double y, double z);
	~Vec3();

	Vec3 operator +(const Vec3& V) const;
	Vec3 operator -(const Vec3& V) const;
	Vec3 dot(const Vec3& V) const;
	Vec3 dot(const IntVec3& V) const;
	Vec3 cross(const Vec3& V) const;
	Vec3 multiply(const double& A) const;
	double len() const;
	double lenXY() const;
	double sumOfComponents() const;
	void overwrite(double x, double y, double z);
};

#endif // !VEC3_H