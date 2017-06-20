#pragma once

#include <map>
#include <vector>

#include "MessageHandlerWrapper.h"
#include "ProcessStartEventWrapper.h"
#include "ProcessAnalyzer.h"
#include "EventTypes.h"

using std::map;
using std::vector;

class ProcessesMonitor : public MessageHandlerWrapper, ProcessStartEventWrapper
{
private:
	const HoneypotsManager *honeypotsManager;
	map<unsigned int, ProcessAnalyzer *> processAnalyzers;

	void initProcessAnalyzers();
	void toggleProcess(DWORD pid, int on);
public:
	void addNewProcess(unsigned int processID);
	void checkProcessesLiveness();
	ProcessesMonitor(const HoneypotsManager *honeypotsManager);
	ProcessesMonitor(const HoneypotsManager * honeypotsManager, unsigned int pid); //TODO for debug only
	virtual void report(int pid, LPUWSTR functionName);
	void suspendProcess(int pid);
	void resumeProcess(int pid);
	void endProcess(int pid);
	void updateProcessScore(int pid, ProcessHistory processOperation);
	void alert(int pid, const wstring& functionName);
	map<unsigned int, ProcessAnalyzer *> getAllProcessesAnalyzers();
	~ProcessesMonitor();
	bool isProcessMonitored(int pid);

	virtual void notifyStartEvent(unsigned int pid, unsigned int parentId);

};

class ProcessesMonitorException : public exception {};
