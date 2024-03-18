#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <mutex>
#include "../../lib/include/Process.h"

class DataContainer;

class Processor : public Process {
public:
	std::mutex mtx;
	DataContainer* data = nullptr;
	static uint32_t step;
	bool flag;

	Processor();
	~Processor();

	bool process() override;
	bool isReady();
	bool putData(DataContainer* dataContainers);
	std::string getProcessName() override;
};

#endif // !PROCESSOR_H