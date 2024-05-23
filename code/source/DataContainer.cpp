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
	//if (laserPtr->needForNewLayer > 0) {
	//	laserPtr->needForNewLayer -= 1;
	//	meshSectorPtr->addNewLayer();
	//}
	if (timeFlowPtr) timeFlowPtr->advance();
	meshSectorPtr->advance();
	if (ID == (size_t)Config::Processes::count - 1) laserPtr->advance();
	stepsInIsolation++;
	if (stepsInIsolation == maxStepsInIsolation) timeToSync = true;
}

void DataContainer::moveDownVaporizedElems() {
	meshSectorPtr->moveDownVaporizedElems();
}

void DataContainer::addNewLayerOfPowder() {
	meshSectorPtr->addNewLayerOfPowder();
}

void DataContainer::sync(MeshSector* meshSectors) {
	meshSectorPtr->syncBorders(meshSectors);
	stepsInIsolation = 0;
	timeToSync = false;
}

void DataContainer::syncContainers(MeshSector* meshSectors, DataContainer* dataContainers) {
	Timer timer = Timer();
	timer.start();
	for (size_t i = 0; i < Config::Processes::count; i++) {
		dataContainers[i].sync(meshSectors);
	}
}