#include <format>
#include <iostream>
#include <ctime>
#include <stdint.h>
#include <math.h>
#include "../include/Timer.h"

Timer::Timer() {
	timeCumulative = 0.0;
	timeOfPreviousIteration = 0.0;
	timeLeftPrecalc = 0.0;
	timeOfStart = 0.0;
	timeSummarized = 0.0;
	previousIterationNumber = 0;
}

Timer::~Timer() {

}

void Timer::start() {
	timeOfStart = timeMSK();
	timeOfPreviousIteration = timeOfStart;
}

void Timer::restart() {
	start();
	timeSummarized = 0.0;
}

void Timer::update() {
	timeCumulative = timeMSK() - timeOfStart;
}

void Timer::pause() {
	update();
	timeSummarized += timeCumulative;
}

void Timer::resume() {
	start();
}

uint32_t Timer::today(double seconds) const {
	return (uint32_t)(floor(seconds / 86400.0) * 86400.0);
}

double Timer::timeMSK() const {
	std::timespec timeSpec;
	if (std::timespec_get(&timeSpec, TIME_UTC) == 0) {
		return 10;
	}
	double rawTime = timeSpec.tv_sec + 1e-9 * timeSpec.tv_nsec + 3600.0 * 3.0;
	return rawTime;
}

double Timer::left(uint32_t iterationsNow, uint32_t iterationsTotal) {
	double MSK = timeMSK();
	double timeDelta = MSK - timeOfPreviousIteration;
	uint32_t iterationsLeft = iterationsTotal - iterationsNow;
	double timeLeft = timeDelta * (double)iterationsLeft;
	if (previousIterationNumber == iterationsNow) {
		timeLeft = timeLeftPrecalc;
	}
	else {
		timeOfPreviousIteration = MSK;
		previousIterationNumber = iterationsNow;
		timeLeftPrecalc = timeLeft;
	}
	return timeLeft;

}

std::string Timer::formatElapsed() {
	update();
	std::string formatElapsedTime = "Elapsed " + formatTime(timeCumulative);
	return formatElapsedTime;
}

std::string Timer::formatLeft(uint32_t iterationsNow, uint32_t iterationsTotal) {
	std::string formatLeft = "Left " + formatTime(left(iterationsNow, iterationsTotal));
	return formatLeft;
}

std::string Timer::formatETA(uint32_t iterationsNow, uint32_t iterationsTotal) {
	update();
	double timeWODays = timeOfStart - today(timeOfStart);
	double timeElapsed = timeCumulative;
	double timeLeft = left(iterationsNow, iterationsTotal);
	std::string timeETA = "ETA " + formatTime(timeWODays + timeElapsed + timeLeft);
	return timeETA;
}

std::string Timer::formatTime(double seconds) const {
	double d_ref = 86400.0;
	double h_ref = 3600.0;
	double m_ref = 60.0;
	double d = floor(seconds / d_ref);
	double secMD = seconds - d * d_ref;
	double h = floor(secMD / h_ref);
	double secMDMH = secMD - h * h_ref;
	double m = floor(secMDMH / m_ref);
	double s = secMDMH - m * m_ref;
	std::string dString = std::to_string((uint32_t)d);
	std::string hString = std::to_string((uint32_t)h);
	std::string mString = std::to_string((uint32_t)m);
	std::string sString = std::to_string(s);
	if (h < 10) hString = '0' + hString;
	if (m < 10) mString = '0' + mString;
	if (s < 10) sString = '0' + sString;
	std::string timeString = std::format("{} {}:{}:{:.5}", dString, hString, mString, sString);
	return timeString;
}