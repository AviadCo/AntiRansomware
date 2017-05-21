#pragma once

#include <map>

#include "ProcessAnalyzer.h"
#include "EventTypes.h"

using std::map;

class ProcessesMonitor
{
private:
	const HoneypotsManager *honeypotsManager;
	map<unsigned int, ProcessAnalyzer> processAnalyzers;

	void initProcessAnalyzers();
public:
	void addNewProcess(unsigned int processID);
	void checkProcessesLiveness();
	ProcessesMonitor(const HoneypotsManager *honeypotsManager);
	~ProcessesMonitor();
};

class ProcessesMonitorException : public exception {};
