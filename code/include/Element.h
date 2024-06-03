#ifndef ELEMENT_H
#define ELEMENT_H

#include <stdint.h>
#include "../../lib/include/Vec.h"
#include "Neighbours.h"

class Mesh;
class MeshSector;
class Laser;

enum State { powder, liquid, solid, vapor };

class Elem {
public:
	uint32_t ID = 0;
	uint32_t globalID = 0;
	uint32_t* vertices = nullptr;
	Neighbours neighbours;
	Neighbours neighboursTruncated;
	Vec3I onSurface;
	Vec3 vec;
	Vec3I index;
	Vec3 elemScaleVec;
	Vec3 nodeScaleVec;
	State state = static_cast<State>(0);
	uint32_t underLaser = 0;
	uint32_t timesMelted = 0;
	uint32_t timesVaporized = 0;
	bool meltedThisTime = false;
	bool vaporizedThisTime = false;
	bool wasMoved = false;
	bool mayBeUnderLaser = false;
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
			double vapor = 0.0;
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
			struct Vapor {
				double mc = 0.0;
				double mcRev = 0.0;
			} vapor;
			struct Enthalpy {
				double minusRegular = 0.0;
				double plusRegular = 0.0;
				double minusPowder = 0.0;
				double plusPowder = 0.0;
				double minusVapor = 0.0;
				double plusVapor = 0.0;
			} enthalpy;
		} energy;
	} localConfig;

	~Elem();

	bool init(Elem* elems, uint32_t ID, const Vec3I& INDEX_VECTOR, const Neighbours& NEIGHBOURS, const Neighbours& NEIGHBOURS_TRUNCATED, const uint32_t STATE);
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
	void updateKandT();
	void calcStep1(const Laser* LASER);
	void calcStep2();
};

#endif // !ELEMENT_H