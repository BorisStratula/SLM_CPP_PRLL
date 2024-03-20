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
	IntVec3 onSurface;
	Vec3 vec;
	IntVec3 index;
	Vec3 elemScaleVec;
	Vec3 nodeScaleVec;
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
	struct LocalConfig {
		struct Geometry {
			Vec3 step;
			Vec3 stepRev;
			Vec3 stepCoeff;
			Vec3 surfaceArea;
		} geometry;
		struct Mass {
			double powder;
			double liquid;
			double solid;
		} mass;
		struct Energy {
			struct Powder {
				double mc;
				double mcRev;
			} powder;
			struct Liquid {
				double mc;
				double mcRev;
			} liquid;
			struct Solid {
				double mc;
				double mcRev;
			} solid;
			struct Enthalpy {
				double minusRegular;
				double plusRegular;
				double minusPowder;
				double plusPowder;
			} enthalpy;
		} energy;
	} localConfig;

	~Elem();

	bool init(Elem* elems, uint32_t ID, const IntVec3& INDEX_VECTOR, const Neighbours& NEIGHBOURS, const Neighbours& NEIGHBOURS_TRUNCATED, const uint32_t STATE);
	bool valid() const;

	double thermalConductivity() const;
	double TofH() const;
	double HofT() const;
	double enthalpyFlow(const Laser* LASER);
	double thetaI(int32_t forwardID, int32_t backwardID, const uint32_t axis, const MeshSector* const MESH_SECTOR) const;
	double thetaF(int32_t forwardID, const MeshSector* const MESH_SECTOR, const uint32_t axis) const;
	double thetaB(int32_t backwardID, const MeshSector* const MESH_SECTOR, const uint32_t axis) const;
	double laserFlux(const Laser* LASER);
	double radiantFlux() const;
	double wallFlux(const Neighbours& NEIGHBOURS) const;
	void vecInit(Elem* elems);
	void fetchConfig();
	void applyMirror();
	void chechState();
	void calcStep1(const Laser* LASER);
	void calcStep2();
};

#endif // !ELEMENT_H