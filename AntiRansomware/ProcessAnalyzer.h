#pragma once

#include <windows.h>

#include <exception>
#include <map>
#include <functional>
#include <string>
#include <inttypes.h>

#include "EventTypes.h"
#include "ProcessPolicy.h"
#include "HoneypotsManager.h"
#include "ProcessHistory.h"

/* includes for hooking */
#include "MessageHandlerWrapper.h"
#include "ProcessHookMonitorWrapper.h"

using std::exception;
using std::map;
using std::wstring;

class ProcessesMonitor;

/* This represent an analyzer for one process in the system */
class ProcessAnalyzer : public FunctionCalledHandlerWrapper
{
	const uint64_t SCORE_THRESHOLD = 150;
private:
	ProcessesMonitor *processesMonitor;
	const HoneypotsManager *honeypotsManager;
	HANDLE processHandle;
	DWORD parentID;
	DWORD injectedByID;
	uint64_t currentScore;
	wstring processName;
	ProcessHistory processHistory;
	bool alreadyNotified; // we want to ask user only once per process

	void setHooks(DWORD proccessID);
	DWORD GetParentProcessID(DWORD dwProcessID);

public:
	ProcessAnalyzer(DWORD proccessID, ProcessesMonitor *processesMonitor, const HoneypotsManager *honeypotsManager);
	~ProcessAnalyzer();
	DWORD getProcessID() const;
	DWORD getParentID();
	bool isProcessStillActive() const;
	bool checkIfAlert() const;
	bool updateScore(ProcessHistory history);
	void parseHookNotification(const wstring& functionName, const wstring& param);
	wstring getProcessName() const;
	unsigned int getCurrentScore() const;

	virtual void report(int pid, LPUWSTR functionName, LPUWSTR param);
};

class ProcessAnalyzerException : public exception {};

