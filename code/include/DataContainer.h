#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include <stdint.h>

class MeshSector;
class Laser;
class TimeFlow;

class DataContainer {
public:
	size_t ID;
	MeshSector* meshSectorPtr;
	MeshSector* allSectorsPtr;
	Laser* laserPtr;
	TimeFlow* timeFlowPtr;
	uint32_t stepsInIsolation;
	uint32_t maxStepsInIsolation;
	bool timeToSync;
	static uint32_t readyCount;

	DataContainer();
	~DataContainer();

	void init(size_t ID, MeshSector* meshSectors, MeshSector* allMeshSectors, Laser* laser, TimeFlow* timeFlow);
	void advance();
	void moveDownVaporizedElems();
	void addNewLayerOfPowder();
	void sync(MeshSector* meshSectors);
	static void syncContainers(MeshSector* meshSectors, DataContainer* dataContainers);
};

#endif // !DATA_CONTAINER_H
