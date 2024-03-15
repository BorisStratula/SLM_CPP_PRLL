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
	vec = vec + velScaled;
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