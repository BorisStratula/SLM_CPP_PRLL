#include "../include/BodyData.h"

BodyData::BodyData(const Mesh* const MESH, const MeshSector* const MESH_SECTORS) {
	totalNodes = MESH->nodesCount;
	totalElems = MESH->elemsCount;
	nodalID = std::vector<double>(totalNodes);
	nodalCoords = std::vector<double>(totalNodes * 3);
	elemID = std::vector<double>(totalElems);
	elemVertices = std::vector<int>(totalElems * 8);
	H = std::vector<double>(totalElems);
	k = std::vector<double>(totalElems);
	T = std::vector<double>(totalElems);
	state = std::vector<double>(totalElems);
	underLaser = std::vector<double>(totalElems);
	qDebug = std::vector<double>(totalElems);
	MDebug = std::vector<double>(totalElems);
	timesMelted = std::vector<double>(totalElems);
	sector = std::vector<double>(totalElems);
	TMax = 0;
	extractMeshData(MESH);
	extractSectorNumber(MESH_SECTORS);
	advance(MESH_SECTORS);
}

BodyData::~BodyData() {
}

void BodyData::advance(const Mesh* const MESH) {
	uint32_t totalElems = MESH->elemsCount;
	TMax = 0;
	for (uint32_t elem = 0; elem < totalElems; elem++) {
		H[elem] = MESH->elems[elem].H;
		k[elem] = MESH->elems[elem].k;
		T[elem] = MESH->elems[elem].T;
		if (T[elem] > TMax) {
			TMax = T[elem];
		}
		underLaser[elem] = MESH->elems[elem].underLaser;
		qDebug[elem] = MESH->elems[elem].qDebug;
		MDebug[elem] = MESH->elems[elem].MDebug;
		timesMelted[elem] = MESH->elems[elem].timesMelted;
		state[elem] = MESH->elems[elem].state;
	}
}

void BodyData::advance(const MeshSector* const MESH_SECTORS) {
	Elem* elem;
	for (size_t sectorID = 0; sectorID < MeshSector::count; sectorID++) {
		for (size_t elemID = 0; elemID < MESH_SECTORS[sectorID].elemsCountBuff; elemID++) {
			elem = &MESH_SECTORS[sectorID].elems[elemID];
			if (elem->persistentSectorID == -1) {
				sector[elem->globalID] = (double)sectorID;
				H[elem->globalID] = elem->H;
				k[elem->globalID] = elem->k;
				T[elem->globalID] = elem->T;
				if (T[elem->globalID] > TMax) {
					TMax = T[elem->globalID];
				}
				underLaser[elem->globalID] = elem->underLaser;
				qDebug[elem->globalID] = elem->qDebug;
				MDebug[elem->globalID] = elem->MDebug;
				timesMelted[elem->globalID] = elem->timesMelted;
				state[elem->globalID] = elem->state;
			}
		}
	}
}

void BodyData::extractMeshData(const Mesh* const MESH) {
	for (size_t node = 0; node < totalNodes; node++) {
		nodalID[node] = (double)node;
		nodalCoords[node * 3 + 0] = (double)MESH->nodes[node].vec.x;
		nodalCoords[node * 3 + 1] = (double)MESH->nodes[node].vec.y;
		nodalCoords[node * 3 + 2] = (double)MESH->nodes[node].vec.z;
	}
	for (size_t elem = 0; elem < totalElems; elem++) {
		elemID[elem] = (double)elem;
		for (uint32_t i = 0; i < 8; i++) {
			elemVertices[elem * 8 + i] = (int)MESH->elems[elem].vertices[i];
		}
	}
}


void BodyData::extractSectorNumber(const MeshSector* const MESH_SECTORS) {
	for (size_t sectorID = 0; sectorID < MeshSector::count; sectorID++) {
		for (size_t elemID = 0; elemID < MESH_SECTORS[sectorID].elemsCountBuff; elemID++) {
			if (MESH_SECTORS[sectorID].elems[elemID].persistentSectorID == -1) {
				sector[MESH_SECTORS[sectorID].elems[elemID].globalID] = (double)sectorID;
			}
		}
	}
}