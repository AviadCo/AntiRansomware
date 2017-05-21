#pragma once

#include <windows.h>

#include <exception>
#include <map>
#include <functional>

#include "EventTypes.h"

using std::exception;
using std::map;

class ProcessesMonitor;

/* This represent an analyzer for one process in the system */
class ProcessAnalyzer
{
private:
	ProcessesMonitor *processesMonitor;
	HANDLE processHandle;
	HANDLE parentHandle;

public:
	ProcessAnalyzer();
	ProcessAnalyzer(DWORD proccessID, ProcessesMonitor *processesMonitor);
	~ProcessAnalyzer();
	DWORD getProcessID();
	DWORD getParentID();
	bool isProcessStillActive() const;
};

class ProcessAnalyzerException : public exception {};

