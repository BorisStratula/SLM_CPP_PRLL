#ifndef BODYDATA_H
#define BODYDATA_H

#include <vector>
#include "Mesh.h"
#include "MeshSector.h"

class BodyData {
public:
	uint32_t totalNodes;
	uint32_t totalElems;
	std::vector<double> nodalID;
	std::vector<double> nodalCoords;
	std::vector<double> elemID;
	std::vector<int> elemVertices;
	std::vector<double> H;
	std::vector<double> k;
	std::vector<double> T;
	std::vector<double> state;
	std::vector<double> underLaser;
	std::vector<double> qDebug;
	std::vector<double> MDebug;
	std::vector<double> timesMelted;
	std::vector<double> timesVaporized;
	std::vector<double> sector;
	double TMax;


	BodyData(const Mesh* const MESH, const MeshSector* const MESH_SECTORS);
	~BodyData();

	void advance(const Mesh* const MESH);
	void advance(const MeshSector* const MESH_SECTORS);
	void extractMeshData(const Mesh* const MESH);
	void extractSectorNumber(const MeshSector* const MESH_SECTORS);
};

#endif // !BODYDATA_H
