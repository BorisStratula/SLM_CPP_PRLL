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
	double volume = 0.0;
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
			double powder = 0.0;
			double liquid = 0.0;
			double solid = 0.0;
		} mass;
		struct Energy {
			struct Powder {
				double mc = 0.0;
				double mcRev = 0.0;
			} powder;
			struct Liquid {
				double mc = 0.0;
				double mcRev = 0.0;
			} liquid;
			struct Solid {
				double mc = 0.0;
				double mcRev = 0.0;
			} solid;
			struct Enthalpy {
				double minusRegular = 0.0;
				double plusRegular = 0.0;
				double minusPowder = 0.0;
				double plusPowder = 0.0;
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
	double thetaAlongAxis(const int32_t FORWARD_ID, const int32_t BACKWARD_ID, const uint32_t AXIS, const MeshSector* const MESH_SECTOR) const;
	double thetaF(const int32_t FORWARD_ID, const MeshSector* const MESH_SECTOR, const uint32_t AXIS) const;
	double thetaB(const int32_t BACKWARD_ID, const MeshSector* const MESH_SECTOR, const uint32_t AXIS) const;
	double thetaIncoming(const int32_t EXTERNAL_ID, const MeshSector* const MESH_SECTOR, const uint32_t AXIS) const;
	double laserFlux(const Laser* LASER);
	double radiantFlux() const;
	double wallFlux(const Neighbours& NEIGHBOURS) const;
	void vecInit(Elem* elems);
	void scaleVecCalculate();
	void fetchConfig();
	void chechState();
	void calcStep1(const Laser* LASER);
	void calcStep2();
};

#endif // !ELEMENT_H