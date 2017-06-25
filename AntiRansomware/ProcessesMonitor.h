#pragma once

#include <map>
#include <vector>

#include "MessageHandlerWrapper.h"
#include "ProcessStartEventWrapper.h"
#include "ProcessStopEventWrapper.h"
#include "ProcessAnalyzer.h"
#include "EventTypes.h"

using std::map;
using std::vector;

class ProcessesMonitor : public MessageHandlerWrapper, ProcessStartEventWrapper, ProcessStopEventWrapper
{
private:
	const HoneypotsManager *honeypotsManager;
	map<unsigned int, ProcessAnalyzer *> processAnalyzers;
	bool updateOccuered;
	int startCounter;
	int stopCounter;

	void initProcessAnalyzers();
	void toggleProcess(DWORD pid, int on);
public:
	void addNewProcess(unsigned int processID);
	void checkProcessesLiveness();
	ProcessesMonitor(const HoneypotsManager *honeypotsManager);
	ProcessesMonitor(const HoneypotsManager * honeypotsManager, unsigned int pid); 
	virtual void report(int pid, LPUWSTR functionName);
	void suspendProcess(int pid);
	void resumeProcess(int pid);
	void endProcess(int pid);
	void updateProcessScore(int pid, ProcessEvent& action);
	void alert(int pid, const wstring& functionName);
	map<unsigned int, ProcessAnalyzer *> getAllProcessesAnalyzers();
	~ProcessesMonitor();
	bool isProcessMonitored(int pid);
	bool isUpdateOccured();
	void updateOccured();

	virtual void notifyStartEvent(unsigned int pid, LPUWSTR processName, unsigned int parentID);
	virtual void notifyStopEvent(unsigned int pid);
	bool monitorHoneypots();
};

class ProcessesMonitorException : public exception {};
