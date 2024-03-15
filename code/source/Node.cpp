#include "../include/Node.h"
#include "../include/Config.h"

Node::Node() {
	ID = 0;
	vec = Vec3();
}

Node::Node(uint32_t _ID, const Vec3& _VEC) {
	ID = _ID;
	vec = _VEC;
}

Node::~Node() {

}

Vec3 Node::nodalVec(uint32_t cornerNumber, const Vec3& ANCHOR_VEC) {
	Vec3 vec;
	switch (cornerNumber)
	{
	case 0:
		vec = ANCHOR_VEC + Vec3(0.0, 0.0, 0.0).dot(Config::Geometry::step);
		break;
	case 1:
		vec = ANCHOR_VEC + Vec3(0.0, 1.0, 0.0).dot(Config::Geometry::step);
		break;
	case 2:
		vec = ANCHOR_VEC + Vec3(1.0, 1.0, 0.0).dot(Config::Geometry::step);
		break;
	case 3:
		vec = ANCHOR_VEC + Vec3(1.0, 0.0, 0.0).dot(Config::Geometry::step);
		break;
	case 4:
		vec = ANCHOR_VEC + Vec3(0.0, 0.0, 1.0).dot(Config::Geometry::step);
		break;
	case 5:
		vec = ANCHOR_VEC + Vec3(0.0, 1.0, 1.0).dot(Config::Geometry::step);
		break;
	case 6:
		vec = ANCHOR_VEC + Vec3(1.0, 1.0, 1.0).dot(Config::Geometry::step);
		break;
	case 7:
		vec = ANCHOR_VEC + Vec3(1.0, 0.0, 1.0).dot(Config::Geometry::step);
		break;
	}
	return vec;
}