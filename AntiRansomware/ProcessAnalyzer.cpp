#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <windows.h>
#include <Tlhelp32.h>
#include <string>
#include <vector>

#include "ProcessAnalyzer.h"
#include "ProcessesMonitor.h"
#include "FileSystemHelper.h"
#include "HelperFunctions.h"
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

	if (currentScore >= SCORE_THRESHOLD) {
		log().info(__FUNCTION__, L"Alert, process with PID " + std::to_wstring(getProcessID()) + L" is act suspiously");
	}

	return currentScore >= SCORE_THRESHOLD;
}

bool ProcessAnalyzer::updateScore(ProcessHistory history)
{
	DWORD parentID = GetParentProcessID(getProcessID());

	processHistory.addHistory(history);

	currentScore = ProcessPolicy::getScoreForHistory(processHistory);

	log().info(__FUNCTION__, L"pid: " + std::to_wstring(getProcessID()) + L" have score of: " + std::to_wstring(currentScore));

	if (parentID != -1 && processesMonitor->isProcessMonitored(parentID)) {
		/* updating parent score also */
		processesMonitor->updateProcessScore(parentID, history);
	}

	processesMonitor->updateOccured();

	return checkIfAlert();
}

void ProcessAnalyzer::parseHookNotification(const wstring & functionName, const wstring & param)
{
	ProcessPolicy::ProcessOperation processOperation;
	ProcessHistory history;

	if (!wcscmp(functionName.c_str(), HookDeleteFileW::name)) {
		if (param.find(L".ant.ram.temp") != std::wstring::npos) {
			//our deletion - ignore
			return;
		}

		processOperation = ProcessPolicy::FILE_DELETE;

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookDeleteFileW::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" file: " + params[FunctionHooksDefinitions::HookDeleteFileW::FILEPATH]);

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}

		history.counterDeleteFileW++;
	}
	else if (!wcscmp(functionName.c_str(), HookReadFile::name)) {


		std::vector<std::wstring> params = StringFunctions::splitParam(param); 

		if (params.empty() || params[FunctionHooksDefinitions::HookReadFile::FILEPATH].compare(L"") == 0) {
			return;
		}

		double entropy = Antropy::calcAntropy(params[FunctionHooksDefinitions::HookReadFile::FILEPATH]);

		log().debug(__FUNCTION__, wstring(HookReadFile::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" on file: " + params[FunctionHooksDefinitions::HookReadFile::FILEPATH]
			/*+ L" file type: " + params[FunctionHooksDefinitions::HookReadFile::FILE_TYPE]*/
			+ L" entropy: " + std::to_wstring(entropy));

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}

		//history.counterWriteFile++;
		//if (params[FunctionHooksDefinitions::HookWriteFile::IS_TYPE_SAME].compare(L"0") == 0) {
		//	history.counterFileTypeChanged++;
		//}
		//if (entropy != -1) {
		//	history.entropyOfWrite += entropy;
		//}
	}
	else if (!wcscmp(functionName.c_str(), HookWriteFile::name)) {
		processOperation = ProcessPolicy::FILE_CHNAGE_CONTENT;

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		double entropy = -1;
		if (params[FunctionHooksDefinitions::HookWriteFile::FILEPATH].compare(L"") != 0) {
			entropy = Antropy::calcAntropy(params[FunctionHooksDefinitions::HookWriteFile::FILEPATH]);
		}


		log().debug(__FUNCTION__, wstring(HookWriteFile::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" on file: " + params[FunctionHooksDefinitions::HookWriteFile::FILEPATH]
			/*+ L" is same type: " + params[FunctionHooksDefinitions::HookWriteFile::IS_TYPE_SAME]
			+ L" similarity: " + params[FunctionHooksDefinitions::HookWriteFile::SIMILARITY]
			+ L" file type: " + params[FunctionHooksDefinitions::HookWriteFile::FILE_TYPE]*/
			+ L" entropy: " + std::to_wstring(entropy));

		if (FileSystemHelper::isTempOrAppData(param)) {
			log().debug(__FUNCTION__, param + L" file is a temp or app data file, ignoring access");

			return;
		}

		history.counterWriteFile++;
		/*if (params[FunctionHooksDefinitions::HookWriteFile::IS_TYPE_SAME].compare(L"0") == 0) {
			history.counterFileTypeChanged++;
		}*/
		if (entropy != -1) {
			history.entropyOfWrite += entropy;
		}
	}
	else if (!wcscmp(functionName.c_str(), HookMoveFileW::name)) {
		processOperation = ProcessPolicy::FILE_RENAME;

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookMoveFileW::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" from: " + params[FunctionHooksDefinitions::HookMoveFileW::FILEPATH_SRC]
			+ L" to: " + params[FunctionHooksDefinitions::HookMoveFileW::FILEPATH_DST]);

		history.counterMoveFileWCounter++;
	}
	else if (!wcscmp(functionName.c_str(), HookCryptEncrypt::name)) {
		processOperation = ProcessPolicy::ENCRYPTION;

		log().debug(__FUNCTION__, wstring(HookCryptEncrypt::name) + L" was called from pid " + std::to_wstring(getProcessID()));

		history.counterCryptEncryptCounter++;
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

		processOperation = ProcessPolicy::PROCESS_INJECTION;

		injectedByID = std::stoi(param);

		history.counterWriteProcessMemoryCounter++;

		//TODO update process monitor if needed
	}
	else if (!wcscmp(functionName.c_str(), HookShellExecuteExW::name)) {
		if ((param.find(L"vssadmin.exe") != std::wstring::npos) && (param.find(L"delete") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLE_SHADOW_COPY;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable shadow copy from pid " + std::to_wstring(getProcessID()));

			history.counterDisableShadowCopy++;
		}
		else if ((param.find(L"wbadmin") != std::wstring::npos) && (param.find(L"disable") != std::wstring::npos) && (param.find(L"backup") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLE_WINDOWS_BACKUP;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable backup from pid " + std::to_wstring(getProcessID()));

			history.counterDisableBackup++;
		}
		else if ((param.find(L"net") != std::wstring::npos) && (param.find(L"stop") != std::wstring::npos) && (param.find(L"srservice") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLE_WINDOWS_RESTORE;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable windows restore from pid " + std::to_wstring(getProcessID()));

			history.counterDisableWindowsRestore++;
		}
		else if ((param.find(L"sc") != std::wstring::npos) && (param.find(L"stop") != std::wstring::npos) && (param.find(L"WinDefend") != std::wstring::npos)) {
			processOperation = ProcessPolicy::DISABLOE_WINDOWS_DEFENDER;

			log().debug(__FUNCTION__, wstring(HookShellExecuteExW::name) + L" is trying to disable windows defender from pid " + std::to_wstring(getProcessID()));

			history.counterDisableWindowsDefender++;
		}
		else {
			/* no suspious activity */
			return;
		}
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
		processOperation = ProcessPolicy::CREATE_REMOTE_THREAD;

		std::vector<std::wstring> params = StringFunctions::splitParam(param);

		log().debug(__FUNCTION__, wstring(HookCreateRemoteThread::name) + L" was called from pid " + std::to_wstring(getProcessID())
			+ L" on pid: " + params[FunctionHooksDefinitions::HookCreateRemoteThread::PID]);

		history.counterCreateRemoteThreadCounter++;

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

	if (updateScore(history) && !alreadyNotified) {
		//TODO uncomment this command
		alreadyNotified = true;
		processesMonitor->alert(getProcessID(), functionName);
	}
}

wstring ProcessAnalyzer::getProcessName() const
{
	return processName;
}

unsigned int ProcessAnalyzer::getCurrentScore() const
{
	return currentScore;
}

void ProcessAnalyzer::report(int pid, LPUWSTR functionName, LPUWSTR param)
{
	parseHookNotification(functionName, param);
}
