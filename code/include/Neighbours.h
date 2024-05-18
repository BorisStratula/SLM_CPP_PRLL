#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

#include <stdint.h>
#include "../../lib/include/Vec.h"

class Neighbours {
public:
	int32_t xMinus;
	int32_t xPlus;
	int32_t yMinus;
	int32_t yPlus;
	int32_t zMinus;
	int32_t zPlus;
	int32_t origin;
	Vec3I onSurface;

	Neighbours();
	Neighbours(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION);
	~Neighbours();

	void truncate();

private:
	int32_t xMinusID(const Vec3I& INDEX_VEC, uint32_t ID) const;
	int32_t xPlusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const;
	int32_t yMinusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const;
	int32_t yPlusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const;
	int32_t zMinusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const;
	int32_t zPlusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const;
};

#endif // !NEIGHBOURS_H