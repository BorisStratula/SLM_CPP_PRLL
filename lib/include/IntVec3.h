#ifndef INTVEC3_H
#define INTVEC3_H

#include <stdint.h>

class Vec3;

class IntVec3 {
public:
	int32_t x;
	int32_t y;
	int32_t z;

	IntVec3();
	IntVec3(int32_t x, int32_t y, int32_t z);
	~IntVec3();

	IntVec3 operator +(const IntVec3& V) const;
	IntVec3 operator -(const IntVec3& V) const;
	IntVec3 operator *(const int32_t a) const;
	IntVec3 operator *(const double a) const;
	IntVec3 dot(const Vec3& V) const;

	int32_t sumOfComponents() const;
};

#endif // !INTVEC3_H

