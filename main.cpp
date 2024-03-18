#include "main.h"

int main()
{
	printIntro();
	Config::readConfig();
	auto timeFlow = TimeFlow();
	auto* laser = new Laser[Config::Processes::inParallel]();
	auto* meshSectors = new MeshSector[Config::Processes::inParallel]();
	auto mesh = Mesh(meshSectors, laser);
	auto bodyData = BodyData(&mesh, meshSectors);
	auto dataWriter = DataWriter(timeFlow, bodyData);
	auto* dataContainers = new DataContainer[Config::Processes::inParallel]();
	auto* processors = new Processor[Config::Processes::inParallel]();

	for (size_t i = 0; i < Config::Processes::inParallel; i++) {
		auto* timeFlowPtr = &timeFlow;
		if (i != Config::Processes::inParallel - 1) timeFlowPtr = nullptr;
		dataContainers[i].init(&meshSectors[i], meshSectors, &laser[i], timeFlowPtr);
		processors[i].putData(&dataContainers[i]);
	}

	while (!timeFlow.stopSimulation) {
		for (Processor::step = 0; Processor::step < 2; Processor::step++) {
			while (1) {
				bool finished = true;
				for (size_t i = 0; i < Config::Processes::inParallel; i++) {
					if (!processors[i].isReady()) {
						finished = false;
						break;
					}
				}
				if (finished) break;
			}
		}
		if (timeFlow.logThisStep) {
			timeFlow.removeFlag();
			bodyData.advance(meshSectors);
			dataWriter.advance(timeFlow, bodyData);
		}
	}

	if (processors) {
		delete[] processors;
	}
	printOutro();
	return 0;
}
