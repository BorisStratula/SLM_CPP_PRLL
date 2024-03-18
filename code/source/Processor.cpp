#include "../include/Processor.h"
#include "../include/DataContainer.h"
#include "../include/Config.h"

uint32_t Processor::step = 0;

Processor::Processor() {
	start();
	flag = false;
}

Processor::~Processor() {
	stop();
}

bool Processor::process() {
	{
		std::lock_guard<std::mutex> mtxLock(mtx);
		if (data) {
			flag = false;
			if (Processor::step == 0) {
				if (data->timeToSync == false) {
					data->advance();
				}
				else flag = true;
			}
			else if (Processor::step == 1) {
				if (data->timeToSync == true) {
					data->sync(data->allSectorsPtr);
				}
				flag = true;
			}
		}
	}
	return true;
}

bool Processor::isReady() {
	std::lock_guard<std::mutex> mtxLock(mtx);
	return flag;
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