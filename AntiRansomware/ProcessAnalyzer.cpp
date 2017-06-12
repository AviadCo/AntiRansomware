#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>

#include "ProcessAnalyzer.h"
#include "ProcessesMonitor.h"
#include "FileSystemHelper.h"
#include "Logger.h"
#include "FunctionCalledHandlerWrapper.h"
#include "FunctionHooksDefinitions.h"

using namespace FunctionHooksDefinitions;

void ProcessAnalyzer::setHooks(DWORD proccessID)
{
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject((int) proccessID, this);
}

DWORD ProcessAnalyzer::GetParentProcessID(DWORD dwProcessID)
{
	DWORD dwParentProcessID = -1;
	HANDLE			hProcessSnapshot;
	PROCESSENTRY32	processEntry32;

	hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnapshot != INVALID_HANDLE_VALUE)
	{
		processEntry32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnapshot, &processEntry32))
		{
			do
			{
				if (dwProcessID == processEntry32.th32ProcessID)
				{
					dwParentProcessID = processEntry32.th32ParentProcessID;
					break;
				}
			} while (Process32Next(hProcessSnapshot, &processEntry32));

			CloseHandle(hProcessSnapshot);
		}
	}

	return dwParentProcessID;
}

ProcessAnalyzer::ProcessAnalyzer(DWORD proccessID, ProcessesMonitor *processesMonitor, const HoneypotsManager *honeypotsManager)
{
	//TODO check if a lower level of access is needed
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, proccessID);
	if (!processHandle) {
		log().error(__FUNCTION__, L"Failed to get process handle, errno: " + GetLastError());

		throw ProcessAnalyzerException();
	}

	parentID = GetParentProcessID(proccessID);
	injectedByID = -1;
	this->processesMonitor = processesMonitor;
	this->honeypotsManager = honeypotsManager;
	currentScore = 0;

	setHooks(proccessID);
}

ProcessAnalyzer::~ProcessAnalyzer()
{
}

DWORD ProcessAnalyzer::getProcessID() const
{
	return GetProcessId(processHandle);
}

DWORD ProcessAnalyzer::getParentID()
{
	return parentID;
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

bool ProcessAnalyzer::checkIfAlert() const
{

	if (currentScore >= 100) {
		log().info(__FUNCTION__, L"Alert, process with PID " + std::to_wstring(getProcessID()) + L" is act suspiously");
	}

	return currentScore >= 100;
}

bool ProcessAnalyzer::updateScore(ProcessPolicy::ProcessOperation processOperation)
{
	DWORD parentID = GetParentProcessID(getProcessID());
	currentScore += ProcessPolicy::getScoreForOperation(processOperation);

	if (parentID != -1) {
		/* updating parent score also */
		processesMonitor->updateProcessScore(parentID, processOperation);
	}

	return checkIfAlert();
}

void ProcessAnalyzer::parseHookNotification(const wstring & functionName, const wstring & param)
{
	ProcessPolicy::ProcessOperation processOperation;

	if (!wcscmp(functionName.c_str(), HookDeleteFileW::name)) {
		processOperation = ProcessPolicy::FILE_DELETE;

		log().debug(__FUNCTION__, wstring(HookDeleteFileW::name) + L" was called from pid " + std::to_wstring(getProcessID()));

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}
	}
	else if (!wcscmp(functionName.c_str(), HookWriteFile::name)) {
		processOperation = ProcessPolicy::FILE_CHNAGE_CONTENT;

		log().debug(__FUNCTION__, wstring(HookWriteFile::name) + L" was called from pid " + std::to_wstring(getProcessID()));

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}
	}
	else if (!wcscmp(functionName.c_str(), HookMoveFileW::name)) {
		processOperation = ProcessPolicy::FILE_RENAME;

		log().debug(__FUNCTION__, wstring(HookMoveFileW::name) + L" was called from pid " + std::to_wstring(getProcessID()));
	}
	else if (!wcscmp(functionName.c_str(), HookCryptEncrypt::name)) {
		processOperation = ProcessPolicy::ENCRYPTION;

		log().debug(__FUNCTION__, wstring(HookCryptEncrypt::name) + L" was called from pid " + std::to_wstring(getProcessID()));
	}
	else if (!wcscmp(functionName.c_str(), HookCreateFileW::name)) {
		log().debug(__FUNCTION__, wstring(HookCreateFileW::name) + L" was called from pid " + std::to_wstring(getProcessID()));

		/* no suspious activity */
		return;
	}
	else if (!wcscmp(functionName.c_str(), HookWriteProcessMemory::name)) {
		log().debug(__FUNCTION__, wstring(HookWriteProcessMemory::name) + L" was called from pid " + std::to_wstring(getProcessID()));

		processOperation = ProcessPolicy::PROCESS_INJECTION;

		injectedByID = std::stoi(param);

		//TODO update process monitor if needed
	}
	else if (!wcscmp(functionName.c_str(), HookShellExecuteEx::name)) {
		if ((param.find(L"vssadmin.exe") != std::wstring::npos) && (param.find(L"delete") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLE_SHADOW_COPY;

			log().debug(__FUNCTION__, wstring(HookShellExecuteEx::name) + L" is trying to disable shadow copy from pid " + std::to_wstring(getProcessID()));
		}
		else if ((param.find(L"wbadmin") != std::wstring::npos) && (param.find(L"disable") != std::wstring::npos) && (param.find(L"backup") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLE_WINDOWS_BACKUP;

			log().debug(__FUNCTION__, wstring(HookShellExecuteEx::name) + L" is trying to disable backup from pid " + std::to_wstring(getProcessID()));
		}
		else if ((param.find(L"net") != std::wstring::npos) && (param.find(L"stop") != std::wstring::npos) && (param.find(L"srservice") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLE_WINDOWS_RESTORE;

			log().debug(__FUNCTION__, wstring(HookShellExecuteEx::name) + L" is trying to disable windows restore from pid " + std::to_wstring(getProcessID()));
		}
		else if ((param.find(L"sc") != std::wstring::npos) && (param.find(L"stop") != std::wstring::npos) && (param.find(L"WinDefend") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLOE_WINDOWS_DEFENDER;

			log().debug(__FUNCTION__, wstring(HookShellExecuteEx::name) + L" is trying to disable windows defender from pid " + std::to_wstring(getProcessID()));
		}
		else {
			/* no suspious activity */
			return;
		}
	}
	else if (!wcscmp(functionName.c_str(), HookCreateProcess::name)) {
		log().debug(__FUNCTION__, wstring(HookCreateProcess::name) + L" was called from pid " + std::to_wstring(getProcessID()) +
					L" and created process with pid " + param);

		processesMonitor->addNewProcess(std::stoi(param));

		/* no suspious activity */
		return;
	}
	else if (!wcscmp(functionName.c_str(), HookCreateRemoteThread::name)) {
		processOperation = ProcessPolicy::CREATE_REMOTE_THREAD;

		log().debug(__FUNCTION__, wstring(HookCreateRemoteThread::name) + L" was called from pid " + std::to_wstring(getProcessID()));
	}
	else {
		log().error(__FUNCTION__, L"Unsupported function name: " + functionName);

		return;
	}

	updateScore(processOperation);
}

void ProcessAnalyzer::report(int pid, LPUWSTR functionName, LPUWSTR param)
{
	parseHookNotification(functionName, param);
}
