#ifndef MESHBLOCK_H
#define MESHBLOCK_H

#include "../../lib/include/IntVec3.h"

class Elem;
class Laser;

class MeshSector {
public:
	static uint32_t count;
	IntVec3 anchor;
	IntVec3 anchorBuff;
	IntVec3 resolution;
	IntVec3 resolutionBuff;
	uint32_t elemsCount;
	uint32_t elemsCountBuff;
	uint32_t vacantElemID;
	Elem* elems;
	uint32_t* volatileElemsIDs;
	uint32_t volatileElemsCount;
	int32_t* lookUpTable;
	Laser* laserPtr;
	MeshSector* meshSectorsPtr;
	uint32_t powderElemsCount;

	MeshSector();
	~MeshSector();

	void init(const IntVec3& anchor, const IntVec3& resolution);
	void advance();
	void getThisElem(Elem* elem, uint32_t sectorOrBuffer);
	void copyThisElem(const Elem* elem);
	void neighboursFix(Elem* elem);
	void checkIfElemIsOnBorder(Elem* elem);
	void syncBorders(const MeshSector* meshSectors);
	void addNewLayerOfPowder();



};

#endif // !MESHBLOCK_H