#include "stdafx.h"

#include "ProcessesMonitor.h"
#include "ProcessHistory.h"
#include "Logger.h"
#include "ProcessHookMonitorWrapper.h"

#include <psapi.h>
#include <windows.h>
#include <thread> 

#define MAX_NUMBER_OF_PROCESSES 300

using std::map;

void ProcessesMonitor::initProcessAnalyzers()
{
	DWORD pProcessIDs[MAX_NUMBER_OF_PROCESSES];
	DWORD numOfIDs;

	processAnalyzers = map<unsigned int, ProcessAnalyzer *>();

	if (!EnumProcesses(pProcessIDs, MAX_NUMBER_OF_PROCESSES * sizeof(DWORD), &numOfIDs)) {
		log().error(__FUNCTION__, L"Failed to get process IDs, errno: " + GetLastError());

		throw ProcessesMonitorException();
	}

	/* numOfIDs returned in bytes */
	numOfIDs /= sizeof(DWORD);

	for (unsigned int i = 0; i < numOfIDs; ++i) {
		HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pProcessIDs[i]);
		if (!processHandle || pProcessIDs[i] == GetCurrentProcessId()) {
			/* no need to inject on self process or processes we can't open */
			continue;
		}
		else {
			CloseHandle(processHandle);
		}

		try {
			processAnalyzers[pProcessIDs[i]] = new ProcessAnalyzer(pProcessIDs[i], this, honeypotsManager);
		}
		catch (ProcessAnalyzerException e) {
			log().error(__FUNCTION__, L"Failed to init events notifier");

			throw ProcessesMonitorException();
		}
	}
}

void ProcessesMonitor::addNewProcess(unsigned int processID)
{
	processAnalyzers[processID] = new ProcessAnalyzer(processID, this, honeypotsManager);
}

void ProcessesMonitor::checkProcessesLiveness()
{
	for (auto const processAnalyzer : processAnalyzers) {
		try {
			if (!processAnalyzer.second->isProcessStillActive()) {			
				processAnalyzers.erase(processAnalyzer.first);
				delete processAnalyzer.second;
				updateOccured();
			}
		}
		catch (ProcessAnalyzerException e) {
			log().error(__FUNCTION__, L"Failed to check if processes are alive");

			throw ProcessesMonitorException();
		}
	}
}

ProcessesMonitor::ProcessesMonitor(const HoneypotsManager * honeypotsManager)
{
	ProcessHookMonitorWrapper::ProcessTraceWrapper::listenProcessesCreation(this);
	//ProcessHookMonitorWrapper::ProcessTraceWrapper::listenProcessesTermination(this);
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(this);

	this->honeypotsManager = honeypotsManager;

	initProcessAnalyzers();
	
	updateOccuered = true;
	startCounter = 0;
	stopCounter = 0;
}

//TODO remove this function after debug
ProcessesMonitor::ProcessesMonitor(const HoneypotsManager * honeypotsManager, unsigned int pid)
{
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(this);

	this->honeypotsManager = honeypotsManager;

	processAnalyzers = map<unsigned int, ProcessAnalyzer *>();

	updateOccuered = true;
	startCounter = 0;
	stopCounter = 0;

	try {
		processAnalyzers[pid] = new ProcessAnalyzer(pid, this, honeypotsManager);
	}
	catch (ProcessAnalyzerException e) {
		log().error(__FUNCTION__, L"Failed to init events notifier");

		throw ProcessesMonitorException();
	}
}

void ProcessesMonitor::report(int pid, LPUWSTR functionName)
{
	log().info(__FUNCTION__, wstring(L"msg from ") + std::to_wstring(pid) + wstring(L": ") + functionName);
}

typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG(NTAPI *NtResumeProcess)(IN HANDLE ProcessHandle);

void ProcessesMonitor::toggleProcess(DWORD pid, int on)
{
	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(GetModuleHandle(L"ntdll"), "NtSuspendProcess");
	NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(GetModuleHandle(L"ntdll"), "NtResumeProcess");

	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (on)
	{
		pfnNtResumeProcess(processHandle);
	}
	else
	{
		pfnNtSuspendProcess(processHandle);
	}

	CloseHandle(processHandle);
}

void ProcessesMonitor::suspendProcess(int pid)
{
	toggleProcess(pid, false);
}

void ProcessesMonitor::resumeProcess(int pid)
{
	toggleProcess(pid, true);
}

void ProcessesMonitor::endProcess(int pid)
{
	HANDLE processHandle;

	processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

	TerminateProcess(processHandle, 1);

	CloseHandle(processHandle);
}

void ProcessesMonitor::updateProcessScore(int pid, ProcessEvent& action)
{
	if (processAnalyzers.at(pid) != NULL) {
		processAnalyzers.at(pid)->updateScore(action);

		updateOccured();
	}
}

void ProcessesMonitor::alert(int pid, const wstring & functionName)
{
	log().info(__FUNCTION__, functionName + L" caused for process ID " + std::to_wstring(pid) + L" to be suspicious");
	
	suspendProcess(pid);
	int btn = MessageBox(0, (L"Process ID " + std::to_wstring(pid) + L" is acting suspicious.\n" +
					         L"Do you want to kill the process?").c_str(), L"AntiRansomware - Suspicious Activity Detected",
							 MB_YESNO + MB_ICONQUESTION);
	if (btn == IDYES) {
		endProcess(pid);

		updateOccured();
	}
	else {
		resumeProcess(pid);
	}

	log().info(__FUNCTION__, L"Process ID " + std::to_wstring(pid) + L" was " + ((btn == IDYES) ? L"killed." : L"resumed."));
}

map<unsigned int, ProcessAnalyzer *> ProcessesMonitor::getAllProcessesAnalyzers()
{
	return processAnalyzers;
}

ProcessesMonitor::~ProcessesMonitor()
{
	ProcessHookMonitorWrapper::ProcessTraceWrapper::unlistenProcessesCreation();
	//ProcessHookMonitorWrapper::ProcessTraceWrapper::unlistenProcessesTermination();
	for (auto const processAnalyzer : processAnalyzers) {
		delete processAnalyzer.second;
	}
}

bool ProcessesMonitor::isProcessMonitored(int pid)
{
	return processAnalyzers.find(pid) != processAnalyzers.end();
}

bool ProcessesMonitor::isUpdateOccured()
{
	bool result = updateOccuered;

	updateOccuered = false;

	return result;
}

void ProcessesMonitor::updateOccured()
{

	updateOccuered = true;

	/*static int refreshRate = 0;

	if (refreshRate % 1000000) {
		updateOccuered = true;
		refreshRate = 0;
	}
	else {
		refreshRate++;
	}*/
	
}

void ProcessesMonitor::notifyStartEvent(unsigned int pid, LPUWSTR processName, unsigned int parentId)
{
	++startCounter;

	if (wcscmp(processName, L"EasyHook64Svc.exe") == 0) {
		return;
	}

	try {
		addNewProcess(pid);
	}
	catch (ProcessAnalyzerException& e) {
		return;
	}
	catch (exception& e) {
		return;
	}

	updateOccured();
}

void ProcessesMonitor::notifyStopEvent(unsigned int pid)
{
	
	++stopCounter;

	try {
		if (processAnalyzers.find(pid) != processAnalyzers.end()) {
			delete processAnalyzers.at(pid);
			processAnalyzers.erase(pid);
		}
	}
	catch (exception& e) {
		return;
	}

	updateOccured();
}

bool ProcessesMonitor::monitorHoneypots()
{
	return honeypotsManager->monitorHoneypots();
}
