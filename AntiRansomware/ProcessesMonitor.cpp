#include "stdafx.h"

#include "ProcessesMonitor.h"
#include "Logger.h"
#include "ProcessHookMonitorWrapper.h"

#include <psapi.h>
#include <windows.h>

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
			if (processAnalyzer.second->isProcessStillActive()) {
				processAnalyzers.erase(processAnalyzer.first);
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
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(this);

	this->honeypotsManager = honeypotsManager;

	initProcessAnalyzers();
	
}

ProcessesMonitor::ProcessesMonitor(const HoneypotsManager * honeypotsManager, unsigned int pid)
{
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(this);

	this->honeypotsManager = honeypotsManager;

	processAnalyzers = map<unsigned int, ProcessAnalyzer *>();

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
	log().info(__FUNCTION__, functionName);
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

ProcessesMonitor::~ProcessesMonitor()
{
	for (auto const processAnalyzer : processAnalyzers) {
		delete processAnalyzer.second;
	}
}
