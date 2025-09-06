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
	state = static_cast<State>(STATE);
	underLaser = 0;
	timesMelted = 0;
	timesVaporized = 0;
	meltedThisTime = false;
	vaporizedThisTime = false;
	wasMoved = false;
	mayBeUnderLaser = false;
	T = Config::Temperature::initial;
	//if (ID == 0) T = 1700.0;
	k = Vec3(thermalConductivity(), thermalConductivity(), thermalConductivity());
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
	if (state == powder) {
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
		a = Config::Energy::Solid::KA;
		b = Config::Energy::Solid::KB;
		if (T > Config::Temperature::melting) {
			a = Config::Energy::Liquid::KA;
			b = Config::Energy::Liquid::KB;
		}
		if (T > Config::Temperature::vaporization) {
			a = Config::Energy::Vapor::KA;
			b = Config::Energy::Vapor::KB;
		}
		return (a * T + b) * sigmoidConst;
	}
}

double Elem::TofH() const {
	double HLiquidMinus;
	double HLiquidPlus;
	double HVaporMinus = localConfig.energy.enthalpy.minusVapor;;
	double HVaporPlus  = localConfig.energy.enthalpy.plusVapor;;
	double mcRev;
	if (state == powder) {
		HLiquidMinus = localConfig.energy.enthalpy.minusPowder;
		HLiquidPlus =  localConfig.energy.enthalpy.plusPowder;
		mcRev =  localConfig.energy.powder.mcRev;
	}
	else {
		HLiquidMinus = localConfig.energy.enthalpy.minusRegular;
		HLiquidPlus =  localConfig.energy.enthalpy.plusRegular;
		mcRev = localConfig.energy.solid.mcRev;
		if (state == liquid) {
			mcRev = localConfig.energy.liquid.mcRev;
		}
		if (state == vapor) {
			mcRev = localConfig.energy.vapor.mcRev;
		}

	}
	if      (H < HLiquidMinus) return H * mcRev;
	else if (H < HLiquidPlus)  return Config::Temperature::melting;
	else if (H < HVaporMinus)  return Config::Temperature::melting + (H - HLiquidPlus) * mcRev;
	else if (H < HVaporPlus)   return Config::Temperature::vaporization;
	else                       return Config::Temperature::vaporization + (H - HVaporPlus) * mcRev;
}

double Elem::HofT() const {
	//if (T > Config::Temperature::melting) return localConfig.energy.liquid.mc * (T - Config::Temperature::melting) + localConfig.energy.enthalpy.plusRegular;
	//else if (state == 0)                  return localConfig.energy.powder.mc * T;
	//else                                  return localConfig.energy.solid.mc * T;

	if (T > Config::Temperature::vaporization) return localConfig.energy.vapor.mc * (T - Config::Temperature::vaporization) + localConfig.energy.enthalpy.plusVapor;
	else if (T > Config::Temperature::melting) return localConfig.energy.liquid.mc * (T - Config::Temperature::melting) + localConfig.energy.enthalpy.plusRegular;
	else if (state == powder)                  return localConfig.energy.powder.mc * T;
	else                                       return localConfig.energy.solid.mc * T;
}

double Elem::enthalpyFlow(const Laser* LASER) {
	Vec3 thetaVec = Vec3(
		thetaAlongAxis(neighboursTruncated.xPlus, neighboursTruncated.xMinus, 1, meshSectorPtr),
		thetaAlongAxis(neighboursTruncated.yPlus, neighboursTruncated.yMinus, 2, meshSectorPtr),
		thetaAlongAxis(neighboursTruncated.zPlus, neighboursTruncated.zMinus, 3, meshSectorPtr)
	);
	thetaVec = thetaVec * localConfig.geometry.surfaceArea;
	double theta = thetaVec.x + thetaVec.y + thetaVec.z;
	double q0 = laserFlux(LASER);
	// at top surface q is from laser flux going down it linearly regres'
	// q = a * z + b
	double q = q0 * (1.0 + (vec.z - Config::Geometry::maxZ) * Config::Laser::decayDepthInv);
	q = clamp(q, 0.0, q0);
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
	//double t = (MESH_SECTOR->elems[EXTERNAL_ID].k + k) * (MESH_SECTOR->elems[EXTERNAL_ID].T - T);
	double t = 0.0;
	switch (AXIS) {
	case 1:
		t = (MESH_SECTOR->elems[EXTERNAL_ID].k.x + k.x) * (MESH_SECTOR->elems[EXTERNAL_ID].T - T);
		t = t / (localConfig.geometry.step.x + MESH_SECTOR->elems[EXTERNAL_ID].localConfig.geometry.step.x);
		return t;
	case 2:
		t = (MESH_SECTOR->elems[EXTERNAL_ID].k.y + k.y) * (MESH_SECTOR->elems[EXTERNAL_ID].T - T);
		t = t / (localConfig.geometry.step.y + MESH_SECTOR->elems[EXTERNAL_ID].localConfig.geometry.step.y);
		return t;
	case 3:
		t = (MESH_SECTOR->elems[EXTERNAL_ID].k.z + k.z) * (MESH_SECTOR->elems[EXTERNAL_ID].T - T);
		t = t / (localConfig.geometry.step.z + MESH_SECTOR->elems[EXTERNAL_ID].localConfig.geometry.step.z);
		return t;
	default:
		printf("Wrond axis specified, only 1, 2 and 3 are allowed\n");
		exit(5);
	}
}

double Elem::laserFlux(const Laser* LASER) {
	return LASER->heatToElem(this) * localConfig.geometry.surfaceArea.z;
}

double Elem::radiantFlux() const {
	if (onSurface.sum() == 0) return 0.0;
	else return (localConfig.geometry.surfaceArea * (Vec3)onSurface).sum() * Config::Radiation::fluxConst * (T * T * T * T - Config::Temperature::air4);
}

double Elem::wallFlux(const Neighbours& NEIGHBOURS) const {
	if (onSurface.sum() == 0) return 0.0;
	else {
		double totalFlux = 0.0;
		if         (onSurface.x > 0) totalFlux += localConfig.geometry.surfaceArea.x *  k.x * (T - Config::Temperature::air) * localConfig.geometry.stepRev.x;
		if         (onSurface.y > 0) totalFlux += localConfig.geometry.surfaceArea.y *  k.y * (T - Config::Temperature::air) * localConfig.geometry.stepRev.y;
		if (NEIGHBOURS.zMinus == -1) totalFlux += localConfig.geometry.surfaceArea.z *  k.z * (T - Config::Temperature::air) * localConfig.geometry.stepRev.z;
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
	localConfig.mass.vapor =  localConfig.geometry.step.x * localConfig.geometry.step.y * localConfig.geometry.step.z * Config::Mass::Rho::vapor;
	localConfig.mass.powder = localConfig.mass.solid * Config::Mass::Rho::packing;
	localConfig.energy.solid.mc =  localConfig.mass.solid * Config::Energy::Solid::C;
	localConfig.energy.liquid.mc = localConfig.mass.liquid * Config::Energy::Liquid::C;
	localConfig.energy.vapor.mc =  localConfig.mass.vapor * Config::Energy::Vapor::C;
	localConfig.energy.powder.mc = localConfig.mass.powder * Config::Energy::Solid::C;
	localConfig.energy.solid.mcRev =  1.0 / localConfig.energy.solid.mc;
	localConfig.energy.liquid.mcRev = 1.0 / localConfig.energy.liquid.mc;
	localConfig.energy.vapor.mcRev =  1.0 / localConfig.energy.vapor.mc; 
	localConfig.energy.powder.mcRev = 1.0 / localConfig.energy.powder.mc;
	localConfig.energy.enthalpy.minusRegular = localConfig.mass.solid * Config::Energy::Solid::C * Config::Temperature::melting;
	localConfig.energy.enthalpy.minusPowder =  localConfig.mass.powder * Config::Energy::Solid::C * Config::Temperature::melting;
	localConfig.energy.enthalpy.plusRegular =  localConfig.energy.enthalpy.minusRegular + localConfig.mass.solid * Config::Energy::Enthalpy::fusion;
	localConfig.energy.enthalpy.plusPowder =   localConfig.energy.enthalpy.minusPowder + localConfig.mass.powder * Config::Energy::Enthalpy::fusion;
	localConfig.energy.enthalpy.minusVapor = localConfig.energy.enthalpy.plusRegular + localConfig.mass.liquid * Config::Energy::Liquid::C * (Config::Temperature::vaporization - Config::Temperature::melting);
	localConfig.energy.enthalpy.plusVapor = localConfig.energy.enthalpy.minusVapor + localConfig.mass.liquid * Config::Energy::Enthalpy::vaporization;
}

void Elem::chechState() {
	if (T > Config::Temperature::vaporization) {
		if (state == liquid) {
			state = vapor;
			timesVaporized += 1;
			vaporizedThisTime = true;
		}
	}
	if (T > Config::Temperature::melting and T < Config::Temperature::vaporization) {
		if (state == solid or state == powder) {
			if (state == powder) H = HofT();
			state = liquid;
			timesMelted += 1;
			meltedThisTime = true;
		}
		if (state == vapor) {
			state = liquid;
		}
	}
	if (T < Config::Temperature::melting) {
		if (state == liquid) {
			state = solid;
		}
	}
}

void Elem::updateKandT() {
	T = TofH();
	double kPrecalc = thermalConductivity();
	if (state == vapor) {
		k = Vec3(
			kPrecalc * Config::Energy::Vapor::anisotropyOfK,
			kPrecalc * Config::Energy::Vapor::anisotropyOfK, 
			kPrecalc
		);
	}
	else {
		k = Vec3(kPrecalc, kPrecalc, kPrecalc);
	}
}

void Elem::calcStep1(const Laser* LASER) {
	HFlow = enthalpyFlow(LASER);
}

void Elem::calcStep2() {
	H += HFlow;
	updateKandT();
	chechState();
	updateKandT();
}

