#include <math.h>
#include "../include/Vec3.h"

Vec3::Vec3() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Vec3::Vec3(double vecX, double vecY, double vecZ) {
	x = vecX;
	y = vecY;
	z = vecZ;
}

Vec3::~Vec3() {

}

Vec3 Vec3::operator +(const Vec3& V) const {
	return Vec3(x + V.x, y + V.y, z + V.z);
}

Vec3 Vec3::operator -(const Vec3& V) const {
	return Vec3(x - V.x, y - V.y, z - V.z);
}

Vec3 Vec3::dot(const Vec3& V) const {
	double newX = x * V.x;
	double newY = y * V.y;
	double newZ = z * V.z;
	return Vec3(newX, newY, newZ);
}

Vec3 Vec3::dot(const IntVec3& V) const {
	double newX = x * (double)V.x;
	double newY = y * (double)V.y;
	double newZ = z * (double)V.z;
	return Vec3(newX, newY, newZ);
}

Vec3 Vec3::cross(const Vec3& V) const {
	double newX = y * V.z - z * V.y;
	double newY = z * V.x - x * V.z;
	double newZ = x * V.y - y * V.x;
	return Vec3(newX, newY, newZ);
}

Vec3 Vec3::multiply(const double& A) const {
	return Vec3(A * x, A * y, A * z);
}

double Vec3::len() const {
	return sqrt(x * x + y * y + z * z);
}

void Vec3::overwrite(double newX, double newY, double newZ) {
	x = newX;
	y = newY;
	z = newZ;
}