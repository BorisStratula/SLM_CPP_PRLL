#include <iostream>

#include "../include/DataContainer.h"
#include "../include/MeshSector.h"
#include "../include/Laser.h"
#include "../include/TimeFlow.h"
#include "../include/Config.h"

uint32_t DataContainer::readyCount = 0;

DataContainer::DataContainer() {
	ID = 0;
	meshSectorPtr = nullptr;
	allSectorsPtr = nullptr;
	laserPtr = nullptr;
	timeFlowPtr = nullptr;
	stepsInIsolation = 0;
	maxStepsInIsolation = Config::Geometry::buffer.x;
	timeToSync = false;
}

DataContainer::~DataContainer() {

}

void DataContainer::init(size_t _ID, MeshSector* meshSectors, MeshSector* allMeshSectors, Laser* laser, TimeFlow* timeFlow) {
	ID = _ID;
	meshSectorPtr = meshSectors;
	allSectorsPtr = allMeshSectors;
	laserPtr = laser;
	timeFlowPtr = timeFlow;
	stepsInIsolation = 0;
}

void DataContainer::advance() {
	if (timeFlowPtr) timeFlowPtr->advance();
	meshSectorPtr->advance();
	if (ID == Config::Processes::inParallel - 1) laserPtr->advance();
	stepsInIsolation++;
	if (stepsInIsolation == maxStepsInIsolation) timeToSync = true;
}

void DataContainer::sync(MeshSector* meshSectors) {
	meshSectorPtr->syncBorders(meshSectors);
	stepsInIsolation = 0;
	timeToSync = false;
}

void DataContainer::syncContainers(MeshSector* meshSectors, DataContainer* dataContainers) {
	Timer timer = Timer();
	timer.start();
	for (size_t i = 0; i < Config::Processes::inParallel; i++) {
		dataContainers[i].sync(meshSectors);
	}
}