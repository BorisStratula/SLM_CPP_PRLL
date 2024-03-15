#ifndef TIMER_H
#define TIMER_H

#include <string>


class Timer {
public:
	double timeCumulative;
	double timeOfPreviousIteration;
	double timeLeftPrecalc;
	double timeOfStart;
	double timeSummarized;
	uint32_t previousIterationNumber;

	Timer();
	~Timer();

	void start();
	void restart();
	void update();
	void pause();
	void resume();

	uint32_t today(double seconds) const;
	double timeMSK() const;
	double left(uint32_t iterationsNow, uint32_t iterationTotal);

	std::string formatElapsed();
	std::string formatLeft(uint32_t iterationsNow, uint32_t iterationsTotal);
	std::string formatETA(uint32_t iteratiosnNow, uint32_t iterationsTotal);
	std::string formatTime(double seconds) const;
};

#endif // !TIMER_H