#ifndef DATAWRITER_H
#define DATAWRITER_H

#include <string>
#include "BodyData.h"
#include "TimeFlow.h"

class DataWriter {
public:
	std::string projectDir;
	std::string solutionDir;

	DataWriter(TimeFlow& timeFlow, const BodyData& BODY_DATA);
	~DataWriter() = default;

	void prepareDir() const;
	void advance(TimeFlow& timeFlow, const BodyData& BODY_DATA) const;
	void writeSolutionFile(const std::string& FILE_NAME, const BodyData& BODY_DATA) const;
	void combineSolutionFiles(uint32_t iteration, uint32_t totalIterations) const;
	std::string addPrefixZeroes(uint32_t value, uint32_t maxValue) const;
};

#endif // !DATAWRITER_H
