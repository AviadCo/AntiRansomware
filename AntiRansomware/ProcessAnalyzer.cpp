#include "stdafx.h"

#include "ProcessAnalyzer.h"
#include "ProcessesMonitor.h"
#include "Logger.h"

ProcessAnalyzer::ProcessAnalyzer()
{
}

ProcessAnalyzer::ProcessAnalyzer(DWORD proccessID, ProcessesMonitor *processesMonitor)
{
	//TODO check if a lower level of access is needed
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, proccessID);
	if (!processHandle) {
		log().error(__FUNCTION__, L"Failed to get process handle, errno: " + GetLastError());

		throw ProcessAnalyzerException();
	}

	parentHandle = GetParent((HWND) processHandle);
	if (!parentHandle) {
		log().error(__FUNCTION__, L"Failed to get parent process handle, errno: " + GetLastError());

		throw ProcessAnalyzerException();
	}

	this->processesMonitor = processesMonitor;
}

ProcessAnalyzer::~ProcessAnalyzer()
{
}

DWORD ProcessAnalyzer::getProcessID()
{
	return GetProcessId(processHandle);
}

DWORD ProcessAnalyzer::getParentID()
{
	return GetProcessId(parentHandle);
}

bool ProcessAnalyzer::isProcessStillActive() const
{
	DWORD exitCode;

	if (!GetExitCodeProcess(processHandle, &exitCode)) {
		log().error(__FUNCTION__, L"Failed to get exit code from process , errno: " + GetLastError());

		throw ProcessAnalyzerException();
	}

	return exitCode == STILL_ACTIVE;
}
