#include "stdafx.h"
#include "ProcessAnalyzer.h"
#include "Logger.h"


ProcessAnalyzer::ProcessAnalyzer(DWORD proccessID)
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
