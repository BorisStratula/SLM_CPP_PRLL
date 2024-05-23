#include <iostream>
#include "../include/TimeFlow.h"
#include "../include/Config.h"
#include "../include/functions.h"
#include "../include/Laser.h"

TimeFlow::TimeFlow(const Laser& LASER) {
	start = Config::Time::start;
	end = Config::Time::end;
	step = Config::Time::step;
	double endBasedOnLaser = calculateEndTime(LASER);
	endBasedOnLaser < end ? end = endBasedOnLaser : end = end;
	now = start;
	iteration = 0;
	iterationLogger = 0;
	totalIterations = (uint32_t)std::round((end - start) / step);
	desiredLogEntries = Config::Log::desiredEntries - 1;
	maxDigits = digitsInValue(desiredLogEntries);
	logEvery = (uint32_t)round((double)totalIterations / (double)desiredLogEntries);
	logCount = 0;
	logThisStep = false;
	lastLog = false;
	stopSimulation = false;
	timerGlobal.start();
	timerCalculation.start();
	std::cout << "simulation start time = " << start << "s, end time = " << end << "s, time step = " << step << "s" << std::endl;
	std::cout << "total iterations to do = " << totalIterations << ", iterations to log = " << desiredLogEntries << std::endl;
	std::cout << "~~~~~~" << std::endl;
}

TimeFlow::~TimeFlow() {

}

void TimeFlow::advance() {
	now += step;
	iteration += 1;
	if (iteration % logEvery == 0) {
		logThisStep = true;
		logCount += 1;
		if (logCount == desiredLogEntries) lastLog = true;
	}
}

void TimeFlow::removeFlag() {
	iterationLogger += 1;
	std::cout << info() << '\n';
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

double TimeFlow::calculateEndTime(const Laser& LASER) const {
	double trackCount = (double)(Config::Laser::tracks * Config::Laser::layers);
	double trackLength = LASER.turnaroundLoc - LASER.vec.x;
	double v = Config::Laser::vel.x;
	double time = trackCount * trackLength / v;
	return time;
}