#include "main.h"

int main()
{
	Config::readConfig();
	auto timeFlow = TimeFlow();
	auto* laser = new Laser[Config::Processes::inParallel]();
	auto* meshSectors = new MeshSector[Config::Processes::inParallel]();
	auto mesh = Mesh(meshSectors, laser);
	auto bodyData = BodyData(&mesh, meshSectors);
	//auto dataWriter = DataWriter(timeFlow, bodyData);
	auto* dataContainers = new DataContainer[Config::Processes::inParallel]();
	auto* processors = new Processor[Config::Processes::inParallel]();

	for (size_t i = 0; i < Config::Processes::inParallel; i++) {
		auto* timeFlowPtr = &timeFlow;
		if (i != Config::Processes::inParallel - 1) timeFlowPtr = nullptr;
		dataContainers[i].init(&meshSectors[i], &laser[i], timeFlowPtr);
		processors[i].putData(&dataContainers[i]);
	}

	while (!timeFlow.stopSimulation) {
		while (1) {
			bool finished = true;
			for (size_t i = 0; i < Config::Processes::inParallel; i++) {
				if (!processors[i].isReady()) {
					finished = false;
					break;
				}
			}
			if (finished) break;
			//else std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		DataContainer::syncContainers(meshSectors, dataContainers);
		if (timeFlow.logThisStep) {
			timeFlow.removeFlag();
			bodyData.advance(meshSectors);
			//dataWriter.advance(timeFlow, bodyData);
		}
	}



	// debug block
	//for (size_t i = 0; i < Config::Processes::inParallel; i++) {
	//	if (meshSectors != nullptr) printf("x = %u, y = %u, sx = %u, sy = %u\n", meshSectors[i].anchor.x, meshSectors[i].anchor.y, meshSectors[i].resolution.x, meshSectors[i].resolution.y);
	//}
	//for (size_t i = 0; i < meshSectors[0].elemsCountBuff; i++) {
	//	printf("ID = %i, globalID = %i, sector ID = %i, elem ID = %i, T = %f, k = %f, neighY+ = %i\n", meshSectors[0].elems[i].ID, meshSectors[0].elems[i].globalID, meshSectors[0].elems[i].persistentSectorID, meshSectors[0].elems[i].persistentElemID, meshSectors[0].elems[i].T, meshSectors[0].elems[i].k, meshSectors[0].elems[i].neighboursTruncated.yPlus);
	//}
	//for (size_t i = 0; i < meshSectors[0].volatileElemsCount; i++) {
	//	printf("volatile elem ID = %u\n", meshSectors[0].volatileElemsIDs[i]);
	//}
	//for (size_t i = 0; i < mesh.elemsCount; i++) {
	//	printf("i = %i, LUT = %i\n", i, meshSectors[0].lookUpTable[i]);
	//}
	// end of debug block

	//if (meshSectors) {
	//	delete[] meshSectors;
	//}
	if (processors) {
		delete[] processors;
	}
	return 0;
}
