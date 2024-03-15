#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

#include <stdint.h>
#include "../../lib/include/Vec3.h"
#include "../../lib/include/IntVec3.h"

class Neighbours {
public:
	int32_t xMinus;
	int32_t xPlus;
	int32_t yMinus;
	int32_t yPlus;
	int32_t zMinus;
	int32_t zPlus;
	int32_t origin;
	uint32_t onSurface;

	Neighbours();
	Neighbours(const IntVec3& INDEX_VEC, uint32_t ID, const IntVec3& RESOLUTION);
	~Neighbours();

	int32_t xMinusID(const IntVec3& INDEX_VEC, uint32_t ID) const;
	int32_t xPlusID(const IntVec3& INDEX_VEC, uint32_t ID, const IntVec3& RESOLUTION) const;
	int32_t yMinusID(const IntVec3& INDEX_VEC, uint32_t ID, const IntVec3& RESOLUTION) const;
	int32_t yPlusID(const IntVec3& INDEX_VEC, uint32_t ID, const IntVec3& RESOLUTION) const;
	int32_t zMinusID(const IntVec3& INDEX_VEC, uint32_t ID, const IntVec3& RESOLUTION) const;
	int32_t zPlusID(const IntVec3& INDEX_VEC, uint32_t ID, const IntVec3& RESOLUTION) const;
	void truncate();
};

#endif // !NEIGHBOURS_H