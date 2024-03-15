#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include <stdint.h>

class MeshSector;
class Laser;
class TimeFlow;

class DataContainer {
public:
	MeshSector* meshSectorPtr;
	Laser* laserPtr;
	TimeFlow* timeFlowPtr;
	uint32_t stepsInIsolation;
	uint32_t maxStepsInIsolation;
	bool timeToSync;
	static uint32_t readyCount;

	DataContainer();
	~DataContainer();

	void init(MeshSector* meshSectors, Laser* laser, TimeFlow* timeFlow);
	void advance();
	void sync(MeshSector* meshSectors);
	static void syncContainers(MeshSector* meshSectors, DataContainer* dataContainers);
};

#endif // !DATA_CONTAINER_H
