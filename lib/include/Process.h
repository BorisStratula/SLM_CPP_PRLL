#ifndef PROCESS_H
#define PROCESS_H

#include <thread>
#include <string>

class Process {
protected:
	static int processCount;
private:
	volatile bool initialized = false;
	std::thread thread;
public:
	volatile bool finished = false;
protected:
	bool highPriority = true;
	int processId;
	volatile bool shutdown = false;

public:
	Process();
	virtual ~Process();
	virtual std::string getProcessName() = 0;
	int getProcessId() const;

protected:
	void start();
	void stop();
	virtual bool process() = 0;

private:
	void _process();
};

#endif // !PROCESS_H