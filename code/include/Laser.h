#ifndef LASER_H
#define LASER_H

#include <string>
#include "../../lib/include/Vec.h"

class Elem;

class Laser {
public:
	Vec3 vec;
	Vec3 vel;
	Vec3 velScaled;
	Vec3I position;
	double radius;
	double radiusInvSqr;
	double divInv;
	double power;
	double specificPower;
	double turnaroundLoc;
	double sideStep;
	bool state;
	bool needForNewLayer;

	Laser();
	~Laser();

	void advance();
	void scaleVectorToGeometrySize(const Vec3& LAST_NODE_VEC);
	void moveVectorBasedOnTracksCount();
	double heatToElem(Elem* const ELEM) const;
};

#endif // !LASER_H