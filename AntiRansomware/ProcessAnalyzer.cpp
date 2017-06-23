#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>
#include <string>
#include <vector>
#include <thread>

#include "ProcessAnalyzer.h"
#include "ProcessesMonitor.h"
#include "FileSystemHelper.h"
#include "HelperFunctions.h"
#include "Logger.h"
#include "FunctionCalledHandlerWrapper.h"
#include "FunctionHooksDefinitions.h"
#include "ProcessFileIOEvent.h"
#include "ProcessFileOpEvent.h"
#include "ProcessIntEvent.h"

using namespace FunctionHooksDefinitions;

void ProcessAnalyzer::setHooks(DWORD proccessID)
{
	std::thread first([proccessID, this]()-> void
	{
		try {
			ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject((int)proccessID, this);
		}
		catch (exception& e){

		}
	});
	first.detach();
	
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
					processName = wstring(processEntry32.szExeFile);
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

	alreadyNotified = false;
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

	if (currentScore >= ProcessPolicy::SCORE_THRESHOLD) {
		log().info(__FUNCTION__, L"Alert, process with PID " + std::to_wstring(getProcessID()) + L" is act suspiously");
	}

	bool retVal = currentScore >= ProcessPolicy::SCORE_THRESHOLD;
	return retVal;
}

bool ProcessAnalyzer::updateScore(ProcessEvent& action)
{
	DWORD parentID = GetParentProcessID(getProcessID());

	currentScore = ProcessPolicy::getScoreForHistory(processHistory, action);

	log().info(__FUNCTION__, L"pid: " + std::to_wstring(getProcessID()) + L" have score of: " + std::to_wstring(currentScore));
	log().debug(__FUNCTION__, L"pid: " + std::to_wstring(getProcessID()) + L" history: " + processHistory.toString());
	if (parentID != -1 && processesMonitor->isProcessMonitored(parentID)) {
		/* updating parent score also */
		processesMonitor->updateProcessScore(parentID, action);
	}

	processesMonitor->updateOccured();
	
	bool retVal = checkIfAlert();
	return retVal;
}

void ProcessAnalyzer::parseHookNotification(const wstring & functionName, const wstring & param)
{
	ProcessEvent::ProcessOperationTag processOperation;
	ProcessHistory history;
	ProcessEvent* action = nullptr;

	if (!wcscmp(functionName.c_str(), HookDeleteFileW::name)) {
		if (param.find(L".ant.ram.temp") != std::wstring::npos) {
			//our deletion - ignore
			return;
		}

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookDeleteFileW::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" file: " + params[FunctionHooksDefinitions::HookDeleteFileW::FILEPATH]);

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");
			return;
		}

		action = new ProcessFileOpEvent(ProcessEvent::FILE_DELETE, params[FunctionHooksDefinitions::HookDeleteFileW::FILEPATH]);
	}
	else if (!wcscmp(functionName.c_str(), HookReadFile::name)) {

		std::vector<std::wstring> params = StringFunctions::splitParam(param); 

		if (params.empty() || params[FunctionHooksDefinitions::HookReadFile::FILEPATH].length() == 0) {
			return;
		}

		log().debug(__FUNCTION__, wstring(HookReadFile::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" On file: " + params[FunctionHooksDefinitions::HookReadFile::FILEPATH]
			+ L" File type: " + params[FunctionHooksDefinitions::HookReadFile::FILE_TYPE]
			+ L" Entropy: " + params[FunctionHooksDefinitions::HookReadFile::ENTROPY]
			+ L" Entropy group size: " + params[FunctionHooksDefinitions::HookReadFile::ENTROPY_SIZE]);

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}

		action = new ProcessFileIOEvent(ProcessEvent::FILE_READ, params[FunctionHooksDefinitions::HookReadFile::FILEPATH],
			params[FunctionHooksDefinitions::HookReadFile::FILE_TYPE], params[FunctionHooksDefinitions::HookReadFile::ENTROPY],
			params[FunctionHooksDefinitions::HookReadFile::ENTROPY_SIZE]);
	}
	else if (!wcscmp(functionName.c_str(), HookWriteFile::name)) {
		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookWriteFile::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" On file: " + params[FunctionHooksDefinitions::HookWriteFile::FILEPATH]
			+ L" Is same type: " + params[FunctionHooksDefinitions::HookWriteFile::IS_TYPE_SAME]
			+ L" File type: " + params[FunctionHooksDefinitions::HookWriteFile::FILE_TYPE]
			+ L" Entropy: " + params[FunctionHooksDefinitions::HookWriteFile::ENTROPY]
			+ L" Entropy group size: " + params[FunctionHooksDefinitions::HookWriteFile::ENTROPY_SIZE]);

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}

		action = new ProcessFileIOEvent(ProcessEvent::FILE_WRITE, params[FunctionHooksDefinitions::HookWriteFile::FILEPATH],
			params[FunctionHooksDefinitions::HookWriteFile::FILE_TYPE], params[FunctionHooksDefinitions::HookWriteFile::IS_TYPE_SAME],
			params[FunctionHooksDefinitions::HookWriteFile::ENTROPY], params[FunctionHooksDefinitions::HookWriteFile::ENTROPY_SIZE]);
	}
	else if (!wcscmp(functionName.c_str(), HookMoveFileW::name)) {
		processOperation = ProcessEvent::FILE_RENAME;

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookMoveFileW::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" from: " + params[FunctionHooksDefinitions::HookMoveFileW::FILEPATH_SRC]
			+ L" to: " + params[FunctionHooksDefinitions::HookMoveFileW::FILEPATH_DST]);

		action = new ProcessFileOpEvent(ProcessEvent::FILE_RENAME, params[FunctionHooksDefinitions::HookMoveFileW::FILEPATH_SRC],
			params[FunctionHooksDefinitions::HookMoveFileW::FILEPATH_DST]);
	}
	else if (!wcscmp(functionName.c_str(), HookCryptEncrypt::name)) {
		processOperation = ProcessEvent::ENCRYPTION;

		log().debug(__FUNCTION__, wstring(HookCryptEncrypt::name) + L" was called from pid " + std::to_wstring(getProcessID()));

		//ingore for now
		return;
	}
	else if (!wcscmp(functionName.c_str(), HookCreateFileW::name)) {
		
		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookCreateFileW::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" file: " + params[FunctionHooksDefinitions::HookCreateFileW::FILEPATH]);

		/* no suspious activity */
		return;
	}
	else if (!wcscmp(functionName.c_str(), HookWriteProcessMemory::name)) {

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookWriteProcessMemory::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" to process: " + params[FunctionHooksDefinitions::HookWriteProcessMemory::DESC_PID]);

		injectedByID = std::stoi(param);

		action = new ProcessIntEvent(ProcessEvent::INJECTION, params[FunctionHooksDefinitions::HookWriteProcessMemory::DESC_PID]);

		//TODO update process monitor if needed
	}
	else if (!wcscmp(functionName.c_str(), HookShellExecuteExW::name)) {
		if ((param.find(L"vssadmin.exe") != std::wstring::npos) && (param.find(L"delete") != std::wstring::npos)) {
			processOperation = ProcessEvent::DISABLE_SHADOW_COPY;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable shadow copy from pid " + std::to_wstring(getProcessID()));

		}
		else if ((param.find(L"wbadmin") != std::wstring::npos) && (param.find(L"disable") != std::wstring::npos) && (param.find(L"backup") != std::wstring::npos)) {
			processOperation = ProcessEvent::DISABLE_WINDOWS_BACKUP;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable backup from pid " + std::to_wstring(getProcessID()));

		}
		else if ((param.find(L"net") != std::wstring::npos) && (param.find(L"stop") != std::wstring::npos) && (param.find(L"srservice") != std::wstring::npos)) {
			processOperation = ProcessEvent::DISABLE_WINDOWS_RESTORE;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable windows restore from pid " + std::to_wstring(getProcessID()));

		}
		else if ((param.find(L"sc") != std::wstring::npos) && (param.find(L"stop") != std::wstring::npos) && (param.find(L"WinDefend") != std::wstring::npos)) {
			processOperation = ProcessEvent::DISABLOE_WINDOWS_DEFENDER;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable windows defender from pid " + std::to_wstring(getProcessID()));

		}
		else {
			/* no suspious activity */
			return;
		}

		action = new ProcessEvent(processOperation);
	}
	else if (!wcscmp(functionName.c_str(), HookCreateProcess::name)) {

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		if (param.find(L"ant.ram.temp\\utils") != std::wstring::npos) {
			//our activity - ignore
			return;
		}
		log().debug(__FUNCTION__, wstring(HookCreateProcess::name) + L" was called from pid " + std::to_wstring(getProcessID()) +
			L" and created process with pid " + params[FunctionHooksDefinitions::HookCreateProcess::PID]
			+ L" file exe: " + params[FunctionHooksDefinitions::HookCreateProcess::APPLICATION_NAME]
			+ L" command: " + params[FunctionHooksDefinitions::HookCreateProcess::COMMAND_LINE]);

		processesMonitor->addNewProcess(std::stoi(param));

		/* no suspious activity */
		return;
	}
	else if (!wcscmp(functionName.c_str(), HookCreateRemoteThread::name)) {
		processOperation = ProcessEvent::INJECTION;

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookCreateRemoteThread::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" on pid: " + params[FunctionHooksDefinitions::HookCreateRemoteThread::PID]);

		action = new ProcessIntEvent(ProcessEvent::INJECTION, params[FunctionHooksDefinitions::HookCreateRemoteThread::PID]);

	}
	else if (!wcscmp(functionName.c_str(), HookCreateRemoteThreadEx::name)) {
		/* IGNORE THIS CASE
		processOperation = ProcessPolicy::CREATE_REMOTE_THREAD;

		log().debug(__FUNCTION__, wstring(HookCreateRemoteThreadEx::name) + L" was called from pid " + std::to_wstring(getProcessID()));
		*/
		return;
	}
	else {
		log().error(__FUNCTION__, L"Unsupported function name: " + functionName);

		return;
	}

	bool retVal = updateScore(*action);
	if (retVal && !alreadyNotified) {
		//TODO uncomment this command
		alreadyNotified = true;

		std::thread t1(&(ProcessesMonitor::alert), processesMonitor, getProcessID(), functionName);
		t1.detach();
	}

	delete action;
}

wstring ProcessAnalyzer::getProcessName() const
{
	return L"";
	//return processName;
}

unsigned int ProcessAnalyzer::getCurrentScore() const
{
	return currentScore;
}

void ProcessAnalyzer::report(int pid, LPUWSTR functionName, LPUWSTR param)
{
	parseHookNotification(functionName, param);
}
