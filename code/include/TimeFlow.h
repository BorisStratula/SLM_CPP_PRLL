#ifndef TIMEFLOW_H
#define TIMEFLOW_H

#include <stdint.h>
#include <string>
#include "../../lib/include/Timer.h"

class Laser;

class TimeFlow {
public:
	double start;
	double end;
	double step;
	double now;
	uint32_t iteration;
	uint32_t iterationLogger;
	uint32_t totalIterations;
	uint32_t desiredLogEntries;
	uint32_t maxDigits;
	uint32_t logEvery;
	uint32_t logCount;
	bool logThisStep;
	bool lastLog;
	bool stopSimulation;
	Timer timerGlobal;
	Timer timerCalculation;

	TimeFlow(const Laser& LASER);
	~TimeFlow();

	void advance();
	void removeFlag();
	std::string info();
	double calculateEndTime(const Laser& LASER) const;
};

#endif // !TIMEFLOW_H