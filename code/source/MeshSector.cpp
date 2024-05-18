#include <cstdio>
#include <cmath>

#include "../include/MeshSector.h"
#include "../include/Element.h"
#include "../include/Config.h"

uint32_t MeshSector::count = 0;

MeshSector::MeshSector() {
	anchor = Vec3I();
	anchorBuff = Vec3I();
	resolution = Vec3I();
	resolutionBuff = Vec3I();
	elemsCount = 0;
	elemsCountBuff = 0;
	vacantElemID = 0;
	elems = nullptr;
	volatileElemsIDs = nullptr;
	volatileElemsCount = 0;
	lookUpTable = nullptr;
	laserPtr = nullptr;
	meshSectorsPtr = nullptr;
	powderElemsCount = 0;
}

MeshSector::~MeshSector() {
	printf("MeshSector destructor\n");
	if (elems) {
		delete[] elems;
		elems = nullptr;
	}
	if (volatileElemsIDs) delete[] volatileElemsIDs;
	if (lookUpTable) {
		delete[] lookUpTable;
		lookUpTable = nullptr;
	}
}

void MeshSector::init(const Vec3I& ANCHOR, const Vec3I& RESOLUTION) {
	anchor = ANCHOR;
	anchorBuff = anchor - Config::Geometry::buffer;
	resolution = RESOLUTION;
	resolutionBuff = resolution + Config::Geometry::buffer * 2;
	elemsCount = resolution.x * resolution.y * resolution.z;
	elemsCountBuff = resolutionBuff.x * resolutionBuff.y * resolutionBuff.z;
	elems = new Elem[elemsCountBuff]();
	volatileElemsIDs = new uint32_t[elemsCountBuff - elemsCount];
	powderElemsCount = (uint32_t)std::round(Config::Geometry::powderThickness / Config::Geometry::step.z);
	count++;
}

void MeshSector::advance() {
	for (size_t i = 0; i < vacantElemID; i++) {
		elems[i].calcStep1(laserPtr);
	}
	for (size_t i = 0; i < vacantElemID; i++) {
		elems[i].calcStep2();
	}
}

void MeshSector::getThisElem(Elem* elem, uint32_t sectorOrBuffer) {
	copyThisElem(elem);
	lookUpTable[elem->ID] = vacantElemID;
	if (sectorOrBuffer == 2) {
		volatileElemsIDs[volatileElemsCount] = vacantElemID;
		volatileElemsCount++;
	}
	vacantElemID++;
}

void MeshSector::copyThisElem(const Elem* elem) {
	elems[vacantElemID].ID = vacantElemID;
	elems[vacantElemID].globalID = elem->ID;
	elems[vacantElemID].vertices = nullptr;
	elems[vacantElemID].neighbours = elem->neighbours;
	elems[vacantElemID].neighboursTruncated = elem->neighboursTruncated;
	elems[vacantElemID].onSurface = elem->onSurface;
	elems[vacantElemID].elemScaleVec = elem->elemScaleVec;
	elems[vacantElemID].nodeScaleVec = elem->nodeScaleVec;
	elems[vacantElemID].index = elem->index;
	elems[vacantElemID].vec = elem->vec;
	elems[vacantElemID].localConfig = elem->localConfig;
	elems[vacantElemID].state = elem->state;
	elems[vacantElemID].underLaser = elem->underLaser;
	elems[vacantElemID].timesMelted = elem->timesMelted;
	//elems[vacantElemID].wasProcessed = elem->wasProcessed;
	elems[vacantElemID].T = elem->T;
	elems[vacantElemID].k = elem->k;
	elems[vacantElemID].H = elem->H;
	elems[vacantElemID].HFlow = elem->HFlow;
	elems[vacantElemID].qDebug = elem->qDebug;
	elems[vacantElemID].MDebug = elem->MDebug;
	elems[vacantElemID].volume = elem->volume;
	elems[vacantElemID].meshSectorPtr = this;
	elems[vacantElemID].persistentElemID = elem->persistentElemID;
	elems[vacantElemID].persistentSectorID = elem->persistentSectorID;

	//elems[vacantElemID] = Elem(*elem);
	//elems[vacantElemID].vertices = nullptr;
	//elems[vacantElemID].meshSectorPtr = this;
}

void MeshSector::neighboursFix(Elem* elem) {
	checkIfElemIsOnBorder(elem);
	elem->neighboursTruncated.xPlus = lookUpTable[elem->neighboursTruncated.xPlus];
	elem->neighboursTruncated.xMinus = lookUpTable[elem->neighboursTruncated.xMinus];
	elem->neighboursTruncated.yPlus = lookUpTable[elem->neighboursTruncated.yPlus];
	elem->neighboursTruncated.yMinus = lookUpTable[elem->neighboursTruncated.yMinus];
	elem->neighboursTruncated.zPlus = lookUpTable[elem->neighboursTruncated.zPlus];
	elem->neighboursTruncated.zMinus = lookUpTable[elem->neighboursTruncated.zMinus];
	elem->neighboursTruncated.origin = lookUpTable[elem->neighboursTruncated.origin];
}

void MeshSector::checkIfElemIsOnBorder(Elem* elem) {
	if (elem->index.x + 1 == anchorBuff.x + resolutionBuff.x) elem->neighboursTruncated.xPlus = elem->neighboursTruncated.origin;
	if (elem->index.y + 1 == anchorBuff.y + resolutionBuff.y) elem->neighboursTruncated.yPlus = elem->neighboursTruncated.origin;
	if (elem->index.x == anchorBuff.x) elem->neighboursTruncated.xMinus = elem->neighboursTruncated.origin;
	if (elem->index.y == anchorBuff.y) elem->neighboursTruncated.yMinus = elem->neighboursTruncated.origin;
}

void MeshSector::syncBorders(const MeshSector* meshSectors) {
	uint32_t persistentSectorID;
	uint32_t persistentElemID;
	uint32_t ID;
	for (size_t i = 0; i < volatileElemsCount; i++) {
		ID = volatileElemsIDs[i];
		persistentSectorID = elems[ID].persistentSectorID;
		persistentElemID = elems[ID].persistentElemID;
		elems[ID].state = meshSectors[persistentSectorID].elems[persistentElemID].state;
		elems[ID].underLaser = meshSectors[persistentSectorID].elems[persistentElemID].underLaser;
		elems[ID].timesMelted = meshSectors[persistentSectorID].elems[persistentElemID].timesMelted;
		elems[ID].T = meshSectors[persistentSectorID].elems[persistentElemID].T;
		elems[ID].k = meshSectors[persistentSectorID].elems[persistentElemID].k;
		elems[ID].H = meshSectors[persistentSectorID].elems[persistentElemID].H;
		elems[ID].HFlow = meshSectors[persistentSectorID].elems[persistentElemID].HFlow;
		elems[ID].qDebug = meshSectors[persistentSectorID].elems[persistentElemID].qDebug;
		elems[ID].MDebug = meshSectors[persistentSectorID].elems[persistentElemID].MDebug;
	}
}

void MeshSector::addNewLayerOfPowder() {
	for (size_t powderLayer = 0; powderLayer < powderElemsCount; powderLayer++) {
		for (size_t ID = 0; ID < vacantElemID; ID++) {
			int32_t zPlus = elems[ID].neighbours.zPlus;
			if (zPlus > 0) {
				zPlus = lookUpTable[zPlus];
				elems[ID].state = elems[zPlus].state;
				elems[ID].timesMelted = elems[zPlus].timesMelted;
				elems[ID].T = elems[zPlus].T;
				elems[ID].k = elems[zPlus].k;
				elems[ID].H = elems[zPlus].H * elems[ID].volume / elems[zPlus].volume;
				elems[ID].HFlow = elems[zPlus].HFlow;
				elems[ID].qDebug = elems[zPlus].qDebug;
				elems[ID].MDebug = elems[zPlus].MDebug;
			}
			else {
				elems[ID].state = static_cast<State>(powder);
				elems[ID].timesMelted = 0;
				elems[ID].T = Config::Temperature::initial;
				elems[ID].k = elems[ID].thermalConductivity();
				elems[ID].H = elems[ID].HofT();
				elems[ID].HFlow = 0;
				elems[ID].qDebug = 0;
				elems[ID].MDebug = 0;
			}
		}
	}
}