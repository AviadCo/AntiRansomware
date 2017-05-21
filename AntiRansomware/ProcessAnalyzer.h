#pragma once

#include <windows.h>

#include <exception>
#include <map>
#include <functional>

#include "EventTypes.h"
#include "ProcessPolicy.h"
#include "HoneypotsManager.h"

using std::exception;
using std::map;

class ProcessesMonitor;

/* This represent an analyzer for one process in the system */
class ProcessAnalyzer
{
private:
	ProcessesMonitor *processesMonitor;
	const HoneypotsManager *honeypotsManager;
	HANDLE processHandle;
	HANDLE parentHandle;
	unsigned int currentScore;

	void setHooks(DWORD proccessID);
public:
	ProcessAnalyzer();
	ProcessAnalyzer(DWORD proccessID, ProcessesMonitor *processesMonitor, const HoneypotsManager *honeypotsManager);
	~ProcessAnalyzer();
	DWORD getProcessID();
	DWORD getParentID();
	bool isProcessStillActive() const;
	bool checkIfAlert() const;
	bool updateScore(ProcessPolicy::ProcessOperation processOperation);
};

class ProcessAnalyzerException : public exception {};

