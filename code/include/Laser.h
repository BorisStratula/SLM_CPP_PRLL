#ifndef LASER_H
#define LASER_H

#include <string>
#include "../../lib/include/Vec3.h"

class Elem;

class Laser {
public:
	Vec3 vec;
	Vec3 vel;
	Vec3 velScaled;
	IntVec3 position;
	double radius;
	double power;
	double specificPower;
	double turnaroundLoc;
	double sideStep;
	bool state;
	uint32_t needForNewLayer;

	Laser();
	~Laser();

	void advance();
	void scaleVectorToGeometrySize(const Vec3& LAST_NODE_VEC);
	void moveVectorBasedOnTracksCount();
	double heatToElem(Elem* const ELEM) const;
};

#endif // !LASER_H