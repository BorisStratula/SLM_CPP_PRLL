#ifndef MESH_H
#define MESH_H

#include <stdint.h>
#include "../../lib/include/vec3.h"
#include "../../lib/include/intvec3.h"
#include "Node.h"
#include "Element.h"

class Laser;
class MeshSector;

class Mesh {
public:


	IntVec3 resolution;
	uint32_t powderLayers;
	uint32_t startPowderAtLayer;
	uint32_t nodesCount;
	uint32_t elemsCount;
	Node* nodes = nullptr;
	Elem* elems = nullptr;
	uint32_t currentNodeID;
	uint32_t currentElemID;

	Mesh();
	Mesh(MeshSector* meshSectors, Laser* laser);
	~Mesh();

	void createElement(uint32_t elemID, const IntVec3& INDEX_VEC, const Neighbours& NEIGHBOURS, const Neighbours& NEIGHBOURS_TRUNCATED, const uint32_t STATE);
	void createNode(uint32_t nodeID, uint32_t nodePos, const Vec3& ANCHOR_VEC);
	uint32_t findNodeForElement(uint32_t nodePos, const Vec3& elemVec, const Neighbours& NEIGHBOURS);
	void createMesh();
	void sectorPreprocessor(MeshSector* meshSectors, Laser* laser);
	void sectorGeometryCalculator(MeshSector* meshSectors);
	void sectorLaserAssign(MeshSector* meshSectors, Laser* laser);
	void sectorLookUpTable(MeshSector* meshSectors);
	void sectorPointerToItself(MeshSector* meshSectors);
	void splitMesh(MeshSector* meshSectors);
	uint32_t isElemInsideSector(const Elem* elem, const MeshSector* meshSectors);
	IntVec3 returnResolution() const;
	double overhed(MeshSector* meshSectors);
};

#endif // !MESH_H