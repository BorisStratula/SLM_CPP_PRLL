#define _USE_MATH_DEFINES
#include <math.h>
#include "../include/Laser.h"
#include "../include/Config.h"
#include "../include/Element.h"

Laser::Laser() {
	vec = Config::Laser::vec;
	vel = Config::Laser::vel;
	velScaled = vel.multiply(Config::Time::step);
	radius = Config::Laser::radius;
	power = Config::Laser::power;
	state = Config::Laser::state;
	precalcConst = Config::Geometry::surfaceArea * power / M_PI / pow(radius, 2.0);
}

Laser::~Laser() {

}

void Laser::advance() {
	// z-shaped pattern
	//if (vec.x > 0) vec = vec + velScaled;
	//else vec = vec + Config::Geometry::size.dot(Vec3(1.0, 0.0, 0.0)) - Vec3(0.0, Laser::radius, 0.0);
	// 
	// s-shaped pattern
	vec = vec + velScaled;
	if (vec.x <= 0) {
		vec = vec - Vec3(0.0, Laser::radius, 0.0);
		velScaled = velScaled.dot(Vec3(-1.0, 1.0, 1.0));
		vec = vec + velScaled.multiply(2.0);
	}
	if (vec.x > Config::Geometry::size.x) {
		vec = vec - Vec3(0.0, Laser::radius, 0.0);
		velScaled = velScaled.dot(Vec3(-1.0, 1.0, 1.0));
		vec = vec + velScaled.multiply(2.0);
	}
}

double Laser::heatToElem(Elem* const ELEM) const {
	double distance = (vec - ELEM->vec).len();
	if (!state) {
		return 0;
	}
	else if (distance > radius) {
		ELEM->underLaser = 0;
		return 0;
	}
	else {
		ELEM->underLaser = 1;
		return precalcConst;
	}
}