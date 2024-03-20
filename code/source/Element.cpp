#include "../../lib/include/IntVec3.h"
#include "../include/Element.h"
#include "../include/Config.h"
#include "../include/Mesh.h"
#include "../include/MeshSector.h"
#include "../include/Laser.h"

Elem::~Elem() {
	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}
}

bool Elem::init(uint32_t _ID, const IntVec3& INDEX_VECTOR, const Neighbours& NEIGHBOURS, const Neighbours& NEIGHBOURS_TRUNCATED, const uint32_t STATE) {
	if (vertices != nullptr) return false;
	ID = _ID;
	globalID = _ID;
	vertices = new uint32_t[8];
	neighbours = NEIGHBOURS;
	neighboursTruncated = NEIGHBOURS_TRUNCATED;
	onSurface = neighboursTruncated.onSurface;
	vec = Config::Geometry::step.dot(INDEX_VECTOR);
	index = INDEX_VECTOR;
	state = STATE;
	underLaser = 0;
	timesMelted = 0;
	wasProcessed = false;
	T = Config::Temperature::initial;
	k = thermalConductivity();
	H = HofT();
	HFlow = 0.0;
	qDebug = 0.0;
	MDebug = 0.0;
	meshSectorPtr = nullptr;
	persistentElemID = -1;
	persistentSectorID = -1;
	return true;
}

bool Elem::valid() const {
	return vertices != nullptr;
}

double Elem::thermalConductivity() const {
	double sigmoidConst = 1.0;
	double massConstRev = 1.0;
	if (state == 0) {
		sigmoidConst = Config::Misc::sigmoidConst;
		massConstRev = Config::Mass::Rho::packingRev;
	}
	if (T == Config::Temperature::melting) {
		double a1 = Config::Energy::Solid::KA;
		double b1 = Config::Energy::Solid::KB;
		double a2 = Config::Energy::Liquid::KA;
		double b2 = Config::Energy::Liquid::KB;
		double ks = (a1 * T + b1) * sigmoidConst;
		double kl = a2 * T + b2;
		return ks + (kl - ks) 
			* (H * massConstRev - Config::Energy::Enthalpy::minusRegular) / (Config::Energy::Enthalpy::plusRegular - Config::Energy::Enthalpy::minusRegular);
	}
	else {
		double a;
		double b;
		if (T < Config::Temperature::melting) {
			a = Config::Energy::Solid::KA;
			b = Config::Energy::Solid::KB;
		}
		else {
			a = Config::Energy::Liquid::KA;
			b = Config::Energy::Liquid::KB;
		}
		return (a * T + b) * sigmoidConst;
	}
}

double Elem::TofH() const {
	double HMinus;
	double HPlus;
	double mcRev;
	if (state == 0) {
		HMinus = Config::Energy::Enthalpy::minusPowder;
		HPlus = Config::Energy::Enthalpy::plusPowder;
		mcRev = Config::Energy::Powder::mcRev;
	}
	else {
		HMinus = Config::Energy::Enthalpy::minusRegular;
		HPlus = Config::Energy::Enthalpy::plusRegular;
		if (state == 1) {
			mcRev = Config::Energy::Liquid::mcRev;
		}
		else {
			mcRev = Config::Energy::Solid::mcRev;
		}
	}
	if (H < HMinus) return H * mcRev;
	else if (H > HMinus and H <= HPlus) return Config::Temperature::melting;
	else return Config::Temperature::melting + (H - HPlus) * mcRev;
}

double Elem::HofT() const {
	if (T > Config::Temperature::melting) return Config::Energy::Liquid::mc * (T - Config::Temperature::melting) + Config::Energy::Enthalpy::plusRegular;
	else if (state == 0) return Config::Energy::Powder::mc * T;
	else return Config::Energy::Solid::mc * T;
}

double Elem::enthalpyFlow(const Laser* LASER) {
	wasProcessed = false;
	double thetaX = thetaI(neighboursTruncated.xPlus, neighboursTruncated.xMinus, 1, meshSectorPtr);
	double thetaY = thetaI(neighboursTruncated.yPlus, neighboursTruncated.yMinus, 2, meshSectorPtr);
	double thetaZ = thetaI(neighboursTruncated.zPlus, neighboursTruncated.zMinus, 3, meshSectorPtr);
	double theta = Config::Geometry::surfaceArea * (thetaX + thetaY + thetaZ);
	double q = laserFlux(LASER);
	qDebug = q;
	double M = radiantFlux();
	M += wallFlux(neighbours);
	MDebug = M;
	double FExt = q - M;
	double enthalpyFlow = (theta + FExt) * Config::Time::step;
	return enthalpyFlow;
}

double Elem::thetaI(int32_t forwardID, int32_t backwardID, uint32_t axis, const MeshSector* const MESH_SECTOR) const {
	return (thetaF(forwardID, MESH_SECTOR) - thetaB(backwardID, MESH_SECTOR)) * Config::Geometry::stepCoeff;
}

double Elem::thetaF(int32_t forwardID, const MeshSector* const MESH_SECTOR) const {
	return (MESH_SECTOR->elems[forwardID].k + k) * (MESH_SECTOR->elems[forwardID].T - T);
}

double Elem::thetaB(int32_t backwardID, const MeshSector* const MESH_SECTOR) const {
	return (k + MESH_SECTOR->elems[backwardID].k) * (T - MESH_SECTOR->elems[backwardID].T);
}

double Elem::laserFlux(const Laser* LASER) {
	if (neighbours.zPlus == -1) return LASER->heatToElem(this);
	else return 0.0;
}

double Elem::radiantFlux() const {
	if (onSurface == 0) return 0.0;
	else return onSurface * Config::Geometry::surfaceArea * Config::Radiation::fluxConst * (T * T * T * T - Config::Temperature::air4);
}

double Elem::wallFlux(const Neighbours& NEIGHBOURS) const {
	if (onSurface == 0) return 0.0;
	else {
		double totalFlux = 0.0;
		if (NEIGHBOURS.xMinus == -1 or NEIGHBOURS.xPlus == -1) totalFlux += k * (T - Config::Temperature::air);
		if (NEIGHBOURS.yMinus == -1 or NEIGHBOURS.yPlus == -1) totalFlux += k * (T - Config::Temperature::air);
		if (NEIGHBOURS.zMinus == -1) totalFlux += k * (T - Config::Temperature::air);
		if (NEIGHBOURS.zPlus == -1) totalFlux += 0.022 * (T - Config::Temperature::air);
		return totalFlux * Config::Geometry::surfaceArea * Config::Geometry::stepRev.x;
	}
}

void Elem::chechState() {
	if (T > Config::Temperature::melting) {
		if (state == 2 or state == 0) {
			if (state == 0) HofT();
			state = 1;
			timesMelted += 1;
		}
	}
	if (T < Config::Temperature::melting) {
		if (state == 1) {
			state = 2;
		}
	}
}

void Elem::calcStep1(const Laser* LASER) {
	wasProcessed = false;
	HFlow = enthalpyFlow(LASER);
}

void Elem::calcStep2() {
	if (!wasProcessed) {
		wasProcessed = true;
		H += HFlow;
		T = TofH();
		k = thermalConductivity();
		chechState();
	}
}

