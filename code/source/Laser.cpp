#define _USE_MATH_DEFINES
#include <math.h>
#include "../include/Laser.h"
#include "../include/Config.h"
#include "../include/Element.h"

Laser::Laser() {
	vec = Config::Laser::vec;
	vel = Config::Laser::vel;
	velScaled = vel.multiply(Config::Time::step);
	position = IntVec3();
	radius = Config::Laser::radius;
	power = Config::Laser::power;
	state = Config::Laser::state;
	specificPower = power / M_PI / pow(radius, 2.0);
	turnaroundLoc = Config::Laser::goUntill;
	sideStep = Config::Laser::sideStep;
	needForNewLayer = 0;
}

Laser::~Laser() {

}

void Laser::advance() {
	// z-shaped pattern
	if (vec.x < turnaroundLoc) vec = vec + velScaled;
	else {
		vec.x = Config::Laser::vec.x;
		vec.y = vec.y + sideStep;
		position.x++;
		//if (position.x >= Config::Laser::tracks) {
		//	vec.y = Config::Laser::vec.y;
		//	needForNewLayer = Config::Processes::inParallel;
		//}
	}
}

void Laser::scaleVectorToGeometrySize(const Vec3& LAST_NODE_VEC) {
	vec = vec.dot(LAST_NODE_VEC);
	moveVectorBasedOnTracksCount();
	Config::Laser::vec = vec;
	turnaroundLoc *= LAST_NODE_VEC.x;
} 

void Laser::moveVectorBasedOnTracksCount() {
	vec.y = vec.y - (double)(Config::Laser::tracks - 1) * Config::Laser::sideStep * 0.5;
}

double Laser::heatToElem(Elem* const ELEM) const {
	double distance = (vec - ELEM->vec).lenXY();
	if (!state) {
		return 0;
	}
	else if (distance > radius) {
		ELEM->underLaser = 0;
		return 0;
	}
	else {
		ELEM->underLaser = 1;
		return specificPower;
	}
}