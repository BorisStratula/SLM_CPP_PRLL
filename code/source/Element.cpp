#include <vector>

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

bool Elem::init(Elem* elems, uint32_t _ID, const Vec3I& INDEX_VECTOR, const Neighbours& NEIGHBOURS, const Neighbours& NEIGHBOURS_TRUNCATED, const uint32_t STATE) {
	if (vertices != nullptr) return false;
	ID = _ID;
	globalID = _ID;
	vertices = new uint32_t[8];
	neighbours = NEIGHBOURS;
	neighboursTruncated = NEIGHBOURS_TRUNCATED;
	onSurface = neighboursTruncated.onSurface;
	elemScaleVec = Vec3(1.0, 1.0, 1.0);
	nodeScaleVec = Vec3(1.0, 1.0, 1.0);
	index = INDEX_VECTOR;
	vecInit(elems);
	fetchConfig();
	state = STATE;
	underLaser = 0;
	timesMelted = 0;
	//wasProcessed = false;
	T = Config::Temperature::initial;
	//if (ID == 0) T = 1700.0;
	k = thermalConductivity();
	H = HofT();
	HFlow = 0.0;
	qDebug = 0.0;
	MDebug = 0.0;
	volume = 0.0;
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
	double rhoConstRev = 1.0;
	if (state == 0) {
		sigmoidConst = Config::Misc::sigmoidConst;
		rhoConstRev = Config::Mass::Rho::packingRev;
	}
	if (T == Config::Temperature::melting) {
		double a1 = Config::Energy::Solid::KA;
		double b1 = Config::Energy::Solid::KB;
		double a2 = Config::Energy::Liquid::KA;
		double b2 = Config::Energy::Liquid::KB;
		double ks = (a1 * T + b1) * sigmoidConst;
		double kl = a2 * T + b2;
		double fracTop = (kl - ks) * (H * rhoConstRev - localConfig.energy.enthalpy.minusRegular);
		double fracBot = localConfig.energy.enthalpy.plusRegular - localConfig.energy.enthalpy.minusRegular;
		return ks + fracTop / fracBot;
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
		HMinus = localConfig.energy.enthalpy.minusPowder;
		HPlus =  localConfig.energy.enthalpy.plusPowder;
		mcRev =  localConfig.energy.powder.mcRev;
	}
	else {
		HMinus = localConfig.energy.enthalpy.minusRegular;
		HPlus =  localConfig.energy.enthalpy.plusRegular;
		if (state == 1) {
			mcRev = localConfig.energy.liquid.mcRev;
		}
		else {
			mcRev = localConfig.energy.solid.mcRev;
		}
	}
	if                     (H < HMinus) return H * mcRev;
	else if (H > HMinus and H <= HPlus) return Config::Temperature::melting;
	else                                return Config::Temperature::melting + (H - HPlus) * mcRev;
}

double Elem::HofT() const {
	if (T > Config::Temperature::melting) return localConfig.energy.liquid.mc * (T - Config::Temperature::melting) + localConfig.energy.enthalpy.plusRegular;
	else if (state == 0)                  return localConfig.energy.powder.mc * T;
	else                                  return localConfig.energy.solid.mc * T;
}

double Elem::enthalpyFlow(const Laser* LASER) {
	//wasProcessed = false;
	Vec3 thetaVec = Vec3(
		thetaAlongAxis(neighboursTruncated.xPlus, neighboursTruncated.xMinus, 1, meshSectorPtr),
		thetaAlongAxis(neighboursTruncated.yPlus, neighboursTruncated.yMinus, 2, meshSectorPtr),
		thetaAlongAxis(neighboursTruncated.zPlus, neighboursTruncated.zMinus, 3, meshSectorPtr)
	);
	thetaVec = thetaVec * localConfig.geometry.surfaceArea;
	double theta = thetaVec.x + thetaVec.y + thetaVec.z;
	double q = laserFlux(LASER);
	qDebug = q;
	double M = radiantFlux();
	M += wallFlux(neighbours);
	MDebug = M;
	double FExt = q - M;
	double enthalpyFlow = (theta + FExt) * Config::Time::step;
	return enthalpyFlow;
}

double Elem::thetaAlongAxis(const int32_t FORWARD_ID, const int32_t BACKWARD_ID, const uint32_t AXIS, const MeshSector* const MESH_SECTOR) const {
	//return thetaF(FORWARD_ID, MESH_SECTOR, AXIS) - thetaB(BACKWARD_ID, MESH_SECTOR, AXIS); // old way of calculating flow
	return thetaIncoming(FORWARD_ID, MESH_SECTOR, AXIS) + thetaIncoming(BACKWARD_ID, MESH_SECTOR, AXIS); // new way of calculation flow, less verbose
}

double Elem::thetaIncoming(const int32_t EXTERNAL_ID, const MeshSector* const MESH_SECTOR, const uint32_t AXIS) const {
	// heat flow from external to current elem
	double t = (MESH_SECTOR->elems[EXTERNAL_ID].k + k) * (MESH_SECTOR->elems[EXTERNAL_ID].T - T);
	switch (AXIS) {
	case 1:
		t = t / (localConfig.geometry.step.x + MESH_SECTOR->elems[EXTERNAL_ID].localConfig.geometry.step.x);
		return t;
	case 2:
		t = t / (localConfig.geometry.step.y + MESH_SECTOR->elems[EXTERNAL_ID].localConfig.geometry.step.y);
		return t;
	case 3:
		t = t / (localConfig.geometry.step.z + MESH_SECTOR->elems[EXTERNAL_ID].localConfig.geometry.step.z);
		return t;
	default:
		printf("Wrond axis specified, only 1, 2 and 3 are allowed\n");
		exit(5);
	}
}

double Elem::thetaF(const int32_t FORWARD_ID, const MeshSector* const MESH_SECTOR, const uint32_t AXIS) const {
	// heat flow from forward to current elem
	double t = (MESH_SECTOR->elems[FORWARD_ID].k + k) * (MESH_SECTOR->elems[FORWARD_ID].T - T);
	switch (AXIS) {
	case 1:
		t = t / (localConfig.geometry.step.x + MESH_SECTOR->elems[FORWARD_ID].localConfig.geometry.step.x);
		return t;
	case 2:
		t = t / (localConfig.geometry.step.y + MESH_SECTOR->elems[FORWARD_ID].localConfig.geometry.step.y);
		return t;
	case 3:
		t = t / (localConfig.geometry.step.z + MESH_SECTOR->elems[FORWARD_ID].localConfig.geometry.step.z);
		return t;
	default:
		printf("Wrond axis specified, only 1, 2 and 3 are allowed\n");
		exit(5);
	}
}

double Elem::thetaB(const int32_t BACKWARD_ID, const MeshSector* const MESH_SECTOR, const uint32_t AXIS) const {
	// heat flow from current  to backward elem
	double t = (k + MESH_SECTOR->elems[BACKWARD_ID].k) * (T - MESH_SECTOR->elems[BACKWARD_ID].T);
	switch (AXIS) {
	case 1:
		t = t / (localConfig.geometry.step.x + MESH_SECTOR->elems[BACKWARD_ID].localConfig.geometry.step.x);
		return t;
	case 2:
		t = t / (localConfig.geometry.step.y + MESH_SECTOR->elems[BACKWARD_ID].localConfig.geometry.step.y);
		return t;
	case 3:
		t = t / (localConfig.geometry.step.z + MESH_SECTOR->elems[BACKWARD_ID].localConfig.geometry.step.z);
		return t;
	default:
		printf("Wrond axis specified, only 1, 2 and 3 are allowed\n");
		exit(5);
	}
}

double Elem::laserFlux(const Laser* LASER) {
	if (neighbours.zPlus == -1) return LASER->heatToElem(this) * localConfig.geometry.surfaceArea.z;
	else return 0.0;
}

double Elem::radiantFlux() const {
	if (onSurface.sum() == 0) return 0.0;
	else return (localConfig.geometry.surfaceArea * (Vec3)onSurface).sum() * Config::Radiation::fluxConst * (T * T * T * T - Config::Temperature::air4);
}

double Elem::wallFlux(const Neighbours& NEIGHBOURS) const {
	if (onSurface.sum() == 0) return 0.0;
	else {
		double totalFlux = 0.0;
		if         (onSurface.x > 0) totalFlux += localConfig.geometry.surfaceArea.x *    k * (T - Config::Temperature::air) * localConfig.geometry.stepRev.x;
		if         (onSurface.y > 0) totalFlux += localConfig.geometry.surfaceArea.y *    k * (T - Config::Temperature::air) * localConfig.geometry.stepRev.y;
		if (NEIGHBOURS.zMinus == -1) totalFlux += localConfig.geometry.surfaceArea.z *    k * (T - Config::Temperature::air) * localConfig.geometry.stepRev.z;
		if (NEIGHBOURS.zPlus == -1)  totalFlux += localConfig.geometry.surfaceArea.z * 10.0 * (T - Config::Temperature::air);
		return totalFlux;
	}
}

void Elem::vecInit(Elem* elems) {
	scaleVecCalculate();
	if      (neighbours.xMinus != -1) vec = elems[neighbours.xMinus].vec + Config::Geometry::step * Vec3(1.0, 0.0, 0.0) * elemScaleVec;
	else if (neighbours.yMinus != -1) vec = elems[neighbours.yMinus].vec + Config::Geometry::step * Vec3(0.0, 1.0, 0.0) * elemScaleVec;
	else if (neighbours.zMinus != -1) vec = elems[neighbours.zMinus].vec + Config::Geometry::step * Vec3(0.0, 0.0, 1.0) * elemScaleVec;
	else vec = Vec3(0.0, 0.0, 0.0);
}

void Elem::scaleVecCalculate() {
	Vec3I centralElemIndex = (Vec3I)(Vec3(0.5, 0.5, 1.0) * (Vec3)Config::Geometry::resolution);
	Vec3 elemMult = Vec3(1.0, 1.0, 1.0);
	Vec3 nodeMult = Vec3(1.0, 1.0, 1.0);
	for (size_t i = 0; i < Config::Geometry::coarsen[0].size(); i++) {
		if (index.x <= (centralElemIndex.x - Config::Geometry::coarsen[0][i])) elemMult.x *= 2.0;
		if (index.x < (centralElemIndex.x - Config::Geometry::coarsen[0][i])) nodeMult.x *= 2.0;
		if (index.x > (centralElemIndex.x + Config::Geometry::coarsen[0][i])) elemMult.x *= 2.0;
		if (index.x >= (centralElemIndex.x + Config::Geometry::coarsen[0][i])) nodeMult.x *= 2.0;
	}
	for (size_t i = 0; i < Config::Geometry::coarsen[1].size(); i++) {
		if (index.y <= (centralElemIndex.y - Config::Geometry::coarsen[1][i])) elemMult.y *= 2.0;
		if (index.y < (centralElemIndex.y - Config::Geometry::coarsen[1][i])) nodeMult.y *= 2.0;
		if (index.y > (centralElemIndex.y + Config::Geometry::coarsen[1][i])) elemMult.y *= 2.0;
		if (index.y >= (centralElemIndex.y + Config::Geometry::coarsen[1][i])) nodeMult.y *= 2.0;
	}
	for (size_t i = 0; i < Config::Geometry::coarsen[2].size(); i++) {
		if (index.z <= (centralElemIndex.z - Config::Geometry::coarsen[2][i])) elemMult.z *= 2.0;
		if (index.z < (centralElemIndex.z - Config::Geometry::coarsen[2][i])) nodeMult.z *= 2.0;
		if (index.z > (centralElemIndex.z + Config::Geometry::coarsen[2][i])) elemMult.z *= 2.0;
		if (index.z >= (centralElemIndex.z + Config::Geometry::coarsen[2][i])) nodeMult.z *= 2.0;
	}
	elemScaleVec = elemScaleVec * elemMult;
	nodeScaleVec = nodeScaleVec * nodeMult;
}

void Elem::fetchConfig() {
	localConfig.geometry.step = Config::Geometry::step * nodeScaleVec;
	localConfig.geometry.stepRev = Vec3(
		1 / localConfig.geometry.step.x,
		1 / localConfig.geometry.step.y,
		1 / localConfig.geometry.step.z
	);
	localConfig.geometry.stepCoeff = Vec3(
		0.5 * 0.5 * localConfig.geometry.stepRev.x * localConfig.geometry.stepRev.x,
		0.5 * 0.5 * localConfig.geometry.stepRev.y * localConfig.geometry.stepRev.y,
		0.5 * 0.5 * localConfig.geometry.stepRev.z * localConfig.geometry.stepRev.z
	);
	localConfig.geometry.surfaceArea = Vec3(
		localConfig.geometry.step.y * localConfig.geometry.step.z,
		localConfig.geometry.step.x * localConfig.geometry.step.z,
		localConfig.geometry.step.x * localConfig.geometry.step.y
	);
	localConfig.mass.solid =  localConfig.geometry.step.x * localConfig.geometry.step.y * localConfig.geometry.step.z * Config::Mass::Rho::solid;
	localConfig.mass.liquid = localConfig.geometry.step.x * localConfig.geometry.step.y * localConfig.geometry.step.z * Config::Mass::Rho::liquid;
	localConfig.mass.powder = localConfig.mass.solid * Config::Mass::Rho::packing;
	localConfig.energy.solid.mc =  localConfig.mass.solid * Config::Energy::Solid::C;
	localConfig.energy.liquid.mc = localConfig.mass.liquid * Config::Energy::Liquid::C;
	localConfig.energy.powder.mc = localConfig.mass.powder * Config::Energy::Solid::C;
	localConfig.energy.solid.mcRev =  1 / localConfig.energy.solid.mc;
	localConfig.energy.liquid.mcRev = 1 / localConfig.energy.liquid.mc;
	localConfig.energy.powder.mcRev = 1 / localConfig.energy.powder.mc;
	localConfig.energy.enthalpy.minusRegular = localConfig.mass.solid * Config::Energy::Solid::C * Config::Temperature::melting;
	localConfig.energy.enthalpy.minusPowder =  localConfig.mass.powder * Config::Energy::Solid::C * Config::Temperature::melting;
	localConfig.energy.enthalpy.plusRegular =  localConfig.energy.enthalpy.minusRegular + localConfig.mass.solid * Config::Energy::Enthalpy::fusion;
	localConfig.energy.enthalpy.plusPowder =   localConfig.energy.enthalpy.minusPowder + localConfig.mass.powder * Config::Energy::Enthalpy::fusion;
}

void Elem::chechState() {
	if (T > Config::Temperature::melting) {
		if (state == 2 or state == 0) {
			if (state == 0) H = HofT();
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

void Elem::updateKandT() {
	T = TofH();
	k = thermalConductivity();
}

void Elem::calcStep1(const Laser* LASER) {
	//wasProcessed = false;
	HFlow = enthalpyFlow(LASER);
}

void Elem::calcStep2() {
	H += HFlow;
	updateKandT();
	chechState();
	updateKandT();
	//if (!wasProcessed) {
	//	wasProcessed = true;
	//	
	//}
	//else {
	//	printf("kek lol\n");
	//}
}

