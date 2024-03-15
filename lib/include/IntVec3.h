#ifndef INTVEC3_H
#define INTVEC3_H

#include <stdint.h>

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
	IntVec3 operator *(int32_t a) const;
};

#endif // !INTVEC3_H

