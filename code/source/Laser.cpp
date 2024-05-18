#include <cmath>
#include <numbers>
#include "../include/Laser.h"
#include "../include/Config.h"
#include "../include/Element.h"

Laser::Laser() {
	vec = Config::Laser::vec;
	vel = Config::Laser::vel;
	velScaled = vel * Config::Time::step;
	position = Vec3I();
	radius = Config::Laser::radius;
	radiusInvSqr = 1 / radius / radius;
	divInv = 1 / (-1.0 + std::exp(5));
	power = Config::Laser::power;
	state = Config::Laser::state;
	specificPower = power * std::numbers::inv_pi / std::pow(radius, 2.0);
	turnaroundLoc = Config::Laser::goUntill;
	sideStep = Config::Laser::sideStep;
	needForNewLayer = false;
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
		if (position.x >= (int32_t)Config::Laser::tracks) {
			vec.y = Config::Laser::vec.y;
			needForNewLayer = true;
			position.x = 0;
			position.y++;
		}
	}
}

void Laser::scaleVectorToGeometrySize(const Vec3& LAST_NODE_VEC) {
	vec = vec * LAST_NODE_VEC;
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
		//return specificPower;
		double q = 5.0 * power * std::exp(5.0 * (1.0 - distance * distance * radiusInvSqr)) * std::numbers::inv_pi * radiusInvSqr * divInv;
		return q;
	}
}