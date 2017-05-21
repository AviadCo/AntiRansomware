#pragma once

#include <map>

#include "ProcessAnalyzer.h"
#include "EventTypes.h"

using std::map;

class ProcessesMonitor
{
private:
	map<unsigned int, ProcessAnalyzer> processAnalyzers;

	void initProcessAnalyzers();
public:
	void addNewProcess(unsigned int processID);
	void checkProcessesLiveness();
	ProcessesMonitor();
	~ProcessesMonitor();
};

class ProcessesMonitorException : public exception {};
