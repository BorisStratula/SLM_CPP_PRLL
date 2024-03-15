#include <iostream>
#include "../include/Process.h"

int Process::processCount = 0;

Process::Process() {
	processId = processCount;
	processCount += 1;
}

Process::~Process() {
	if(!finished) {
		std::cout << "Process::~Process Error: process is not finished" << std::endl;
	}
}

int Process::getProcessId() const {
	return processId;
}

void Process::start() {
	if(initialized) return;
	shutdown = false;
	finished = false;
	thread = std::thread(&Process::_process, this);
	initialized = true;
	std::cout << "Process started: \"" << getProcessId() << "_" << getProcessName() << "\"" << std::endl;
}

void Process::stop() {
	if(!initialized) { return; }
	shutdown = true;
	while(!finished) {}
	thread.join();
	initialized = false;
	std::cout << "Process finished: \"" << getProcessId() << "_" << getProcessName() << "\"" << std::endl;
}

void Process::_process() {
	using namespace std::chrono;
	while(!shutdown) {
		if(!process()) break;
		if(!highPriority) std::this_thread::sleep_for(1us);
	}
	finished = true;
}
