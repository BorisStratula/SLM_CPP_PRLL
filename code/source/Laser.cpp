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
	specificPower = power / M_PI / pow(radius, 2.0);
}

Laser::~Laser() {

}

void Laser::advance() {
	// general
	//vec = vec + velScaled;
	
	// z-shaped pattern
	if (vec.x < 0.002) vec = vec + velScaled;
	else {
		vec.x = Config::Laser::vec.x;
		vec.y = vec.y + 2.5 * Config::Laser::radius;
	}
	
	
	// s-shaped pattern
	//vec = vec + velScaled;
	//if (vec.x <= 0) {
	//	vec = vec + Vec3(0.0, Laser::radius, 0.0);
	//	velScaled = velScaled.dot(Vec3(-1.0, 1.0, 1.0));
	//	vec = vec + velScaled.multiply(2.0);
	//}
	//if (vec.x > 6e-4) {
	//	vec = vec + Vec3(0.0, Laser::radius, 0.0);
	//	velScaled = velScaled.dot(Vec3(-1.0, 1.0, 1.0));
	//	vec = vec + velScaled.multiply(2.0);
	//}
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