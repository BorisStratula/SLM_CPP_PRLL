#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include "../../lib/include/Vec3.h"

class Node {
public:
	uint32_t ID;
	Vec3 vec;

	Node();
	Node(uint32_t ID, const Vec3& VEC);
	~Node();

	static Vec3 nodalVec(uint32_t cornerNumber, const Vec3& ANCHOR_VEC, const Vec3& NODE_SCALE_VEC);
};

#endif // !NODE_H