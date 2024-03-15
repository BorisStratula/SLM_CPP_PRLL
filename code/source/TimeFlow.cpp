#include <iostream>
#include "../include/TimeFlow.h"
#include "../include/Config.h"
#include "../include/functions.h"

TimeFlow::TimeFlow() {
	start = Config::Time::start;
	end = Config::Time::end;
	step = Config::Time::step;
	now = start;
	iteration = 0;
	iterationLogger = 0;
	totalIterations = (uint32_t)std::round((end - start) / step);
	desiredLogEntries = Config::Log::desiredEntries - 1;
	maxDigits = digitsInValue(desiredLogEntries);
	logEvery = (uint32_t)round((double)totalIterations / (double)desiredLogEntries);
	logThisStep = false;
	stopSimulation = false;
	timerGlobal.start();
	timerCalculation.start();
}

TimeFlow::~TimeFlow() {

}

void TimeFlow::advance() {
	now += step;
	iteration += 1;
	if (iteration % logEvery == 0) {
		logThisStep = true;
		//iterationLogger += 1;
		//std::cout << info() << '\n';
		//if (iterationLogger == desiredLogEntries) {
		//	stopSimulation = true;
		//}
	}
	//else {
	//	logThisStep = false;
	//}
}

void TimeFlow::removeFlag() {
	iterationLogger += 1;
	std::cout << info() << '\n';
	//std::cout << "calc time" << timerCalculation.formatElapsed() << std::endl;
	if (iterationLogger == desiredLogEntries) {
		stopSimulation = true;
	}
	logThisStep = false;
}

std::string TimeFlow::info() {
	std::string iterationsOutOfTotal = std::to_string(iterationLogger);
	size_t strLen = iterationsOutOfTotal.length();
	while (strLen < maxDigits) {
		iterationsOutOfTotal = ' ' + iterationsOutOfTotal;
		strLen += 1;
	}
	iterationsOutOfTotal += " / " + std::to_string(desiredLogEntries);
	std::string elapsed = timerGlobal.formatElapsed();
	std::string left = timerGlobal.formatLeft(iterationLogger, desiredLogEntries);
	std::string eta = timerGlobal.formatETA(iterationLogger, desiredLogEntries);
	std::string string = iterationsOutOfTotal + " | " + elapsed + " | " + left + " | " + eta;
	return string;
}