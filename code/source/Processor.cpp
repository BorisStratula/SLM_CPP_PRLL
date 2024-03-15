#include "../include/Processor.h"
#include "../include/DataContainer.h"
#include "../include/Config.h"


Processor::Processor() {
	start();
}

Processor::~Processor() {
	stop();
}

bool Processor::process() {
	{
		std::lock_guard<std::mutex> mtxLock(mtx);
		if (data) {
			if (data->timeToSync == false) {
				data->advance();
			}
			//if (data->timeToSync == true) {
			//	DataContainer::readyCount++;
			//	while (1) {
			//		if (DataContainer::readyCount == Config::Processes::inParallel) {
			//			data->sync(data->meshSectorPtr);
			//			DataContainer::readyCount = 0;
			//			break;
			//		}
			//		
			//	}
			//	
			//}
		}
	}
	return true;
}

bool Processor::isReady() {
	std::lock_guard<std::mutex> mtxLock(mtx);
	return data->timeToSync;
}

bool Processor::putData(DataContainer* dataContainers) {
	{
		std::lock_guard<std::mutex> mtxLock(mtx);
		if (data != nullptr) return false;
		data = dataContainers;
	}
	return true;
}

std::string Processor::getProcessName() {
	return "process";
}