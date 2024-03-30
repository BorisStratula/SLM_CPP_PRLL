#include "../include/IntVec3.h"
#include "../include/Vec3.h"

IntVec3::IntVec3() {
	x = 0;
	y = 0;
	z = 0;
}

IntVec3::IntVec3(int32_t newX, int32_t newY, int32_t newZ) {
	x = newX;
	y = newY;
	z = newZ;
}

IntVec3::~IntVec3() {

}

IntVec3 IntVec3::operator +(const IntVec3& V) const {
	return IntVec3(x + V.x, y + V.y, z + V.z);
}

IntVec3 IntVec3::operator -(const IntVec3& V) const {
	return IntVec3(x - V.x, y - V.y, z - V.z);
}

IntVec3 IntVec3::operator *(const int32_t a) const {
	return IntVec3(a * x, a * y, a * z);
}

IntVec3 IntVec3::operator *(const double a) const {
	return IntVec3(int32_t(a * x), int32_t(a * y), int32_t(a * z));
}

IntVec3 IntVec3::dot(const Vec3& V) const {
	return IntVec3(int32_t(x * V.x), int32_t(y * V.y), int32_t(z * V.z));
}

int32_t IntVec3::sumOfComponents() const {
	return x + y + z;
}