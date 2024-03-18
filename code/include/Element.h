#ifndef ELEMENT_H
#define ELEMENT_H

#include <stdint.h>
#include "../../lib/include/Vec3.h"
#include "Neighbours.h"

class Mesh;
class MeshSector;
class Laser;
class IntVec3;

class Elem {
public:
	uint32_t ID = 0;
	uint32_t globalID = 0;
	uint32_t* vertices = nullptr;
	Neighbours neighbours;
	Neighbours neighboursTruncated;
	uint32_t onSurface = 0;
	Vec3 vec;
	IntVec3 index;
	uint32_t state = 0; // 0 == powder, 1 == liquid, 2 == solid
	uint32_t underLaser = 0;
	uint32_t timesMelted = 0;
	bool wasProcessed = false;
	double T = 0.0;
	double k = 0.0;
	double H = 0.0;
	double HFlow = 0.0;
	double qDebug = 0.0;
	double MDebug = 0.0;
	MeshSector* meshSectorPtr = nullptr;
	int32_t persistentElemID = 0;
	int32_t persistentSectorID = 0;

	~Elem();

	bool init(uint32_t ID, const IntVec3& INDEX_VECTOR, const Neighbours& NEIGHBOURS, const Neighbours& NEIGHBOURS_TRUNCATED, const uint32_t STATE);
	bool valid() const;

	double thermalConductivity() const;
	double TofH() const;
	double HofT() const;
	double enthalpyFlow(const Laser* LASER);
	double thetaI(int32_t forwardID, int32_t backwardID, uint32_t axis, const MeshSector* const MESH_SECTOR) const;
	double thetaF(int32_t forwardID, const MeshSector* const MESH_SECTOR) const;
	double thetaB(int32_t backwardID, const MeshSector* const MESH_SECTOR) const;
	double radiantFlux() const;
	double wallFlux(const Neighbours& NEIGHBOURS) const;
	void chechState();
	void calcStep1(const Laser* LASER);
	void calcStep2();
};

#endif // !ELEMENT_H