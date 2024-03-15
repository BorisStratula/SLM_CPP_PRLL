#include <string>

#include "../include/functions.h"

std::vector<uint32_t> findFactors(const uint32_t VALUE) {
	std::vector<uint32_t> factors = std::vector<uint32_t>();
	size_t iMax = (size_t)VALUE / 2 + 1;
	uint32_t a = 1;
	uint32_t b = 1;
	switch (VALUE) {
	case 0:
		break;
	case 1:
		factors.push_back(1);
		factors.push_back(1);
		break;
	case 2:
		factors.push_back(1);
		factors.push_back(2);
		break;
	case 3:
		factors.push_back(1);
		factors.push_back(3);
		break;
	default:
		for (size_t i = 1; i < iMax; i++) {
			if (VALUE % i == 0) {
				a = (uint32_t)i;
				b = VALUE / a;
				if (a > b) continue;
				factors.push_back(a);
				factors.push_back(b);
			}
		}
		break;
	}
	return factors;
}

bool findOptimalRatio(const uint32_t* PROCESS_COUNT, const int32_t* X_SIZE, const int32_t* Y_SIZE, uint32_t* xDiv, uint32_t* yDiv) {
	std::vector<uint32_t> factors = findFactors(*PROCESS_COUNT);
	if (factors.size() == 0) return false;
	double geomRatio;
	double thisRatio;
	double thisToGeom;
	double thisDelta;
	double bestDelta = 1e9;
	size_t bestI = 0;
	if (*X_SIZE >= *Y_SIZE) geomRatio = (double)*X_SIZE / (double)*Y_SIZE;
	else geomRatio = (double)*Y_SIZE / (double)*X_SIZE;
	for (size_t i = 0; i < factors.size(); i += 2) {
		thisRatio = (double)factors[i + 1] / (double)factors[i];
		thisToGeom = thisRatio / geomRatio;
		thisDelta = (thisToGeom - 1.0) * (thisToGeom - 1.0);
		if (thisDelta < bestDelta) {
			bestI = i;
			bestDelta = thisDelta;
		}
	}
	if (*X_SIZE >= *Y_SIZE) {
		*xDiv = factors[bestI + 1];
		*yDiv = factors[bestI];
	}
	else
	{
		*xDiv = factors[bestI];
		*yDiv = factors[bestI + 1];
	}
	return true;
}

uint32_t digitsInValue(const uint32_t value) {
	std::string str = std::to_string(value);
	uint32_t length = 0;
	size_t iMax = str.length();
	for (size_t i = 0; i < iMax; i++) {
		length += 1;
	}
	return length;
}