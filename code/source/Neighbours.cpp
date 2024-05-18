#include "../include/Neighbours.h"


Neighbours::Neighbours() {
	xMinus = 0;
	xPlus = 0;
	yMinus = 0;
	yPlus = 0;
	zMinus = 0;
	zPlus = 0;
	origin = 0;
	onSurface = Vec3I();
}


Neighbours::Neighbours(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) {
	xMinus = xMinusID(INDEX_VEC, ID);
	xPlus = xPlusID(INDEX_VEC, ID, RESOLUTION);
	yMinus = yMinusID(INDEX_VEC, ID, RESOLUTION);
	yPlus = yPlusID(INDEX_VEC, ID, RESOLUTION);
	zMinus = zMinusID(INDEX_VEC, ID, RESOLUTION);
	zPlus = zPlusID(INDEX_VEC, ID, RESOLUTION);
	origin = (int32_t)ID;
	onSurface = Vec3I();
}


Neighbours::~Neighbours() {

}


int32_t Neighbours::xMinusID(const Vec3I& INDEX_VEC, uint32_t ID) const {
	if (INDEX_VEC.x == 0) {
		return -1;
	}
	else {
		return ID - 1;
	}
}

int32_t Neighbours::xPlusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const {
	if (INDEX_VEC.x == RESOLUTION.x - 1) {
		return -1;
	}
	else {
		return ID + 1;
	}
}

int32_t Neighbours::yMinusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const {
	if (INDEX_VEC.y == 0) {
		return -1;
	}
	else {
		return ID - RESOLUTION.x;
	}
}

int32_t Neighbours::yPlusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const {
	if (INDEX_VEC.y == RESOLUTION.y - 1) {
		return -1;
	}
	else {
		return ID + RESOLUTION.x;
	}
}

int32_t Neighbours::zMinusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const {
	if (INDEX_VEC.z == 0) {
		return -1;
	}
	else {
		return ID - RESOLUTION.x * RESOLUTION.y;
	}
}

int32_t Neighbours::zPlusID(const Vec3I& INDEX_VEC, uint32_t ID, const Vec3I& RESOLUTION) const {
	if (INDEX_VEC.z == RESOLUTION.z - 1) {
		return -1;
	}
	else {
		return ID + RESOLUTION.x * RESOLUTION.y;
	}
}


void Neighbours::truncate() {
	if (xMinus == -1) {
		xMinus = origin;
		onSurface.x += 1;
	}
	if (xPlus == -1) {
		xPlus = origin;
		onSurface.x += 1;
	}
	if (yMinus == -1) {
		yMinus = origin;
		onSurface.y += 1;
	}
	if (yPlus == -1) {
		yPlus = origin;
		onSurface.y += 1;
	}
	if (zMinus == -1) {
		zMinus = origin;
		onSurface.z += 1;
	}
	if (zPlus == -1) {
		zPlus = origin;
		onSurface.z += 1;
	}
}