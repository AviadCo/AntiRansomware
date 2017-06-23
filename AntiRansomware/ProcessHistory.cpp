#include "stdafx.h"
#include "ProcessHistory.h"


ProcessHistory::ProcessHistory()
{
	processCreationTime = std::chrono::system_clock::now();
	counterWriteFile = 0;
	counterFileTypeChanged = 0;
	entropyOfRead = 0;
	entropyOfWrite = 0;
	entropySumDelta = 0;
	counterDeleteFileW = 0;
	counterMoveFileWCounter = 0;
	counterCryptEncryptCounter = 0;
	counterDisableShadowCopy = 0;
	counterDisableWindowsRestore = 0;
	counterDisableWindowsDefender = 0;
	counterInjectionCounter = 0;
	counterReadFile = 0;
	timeDiff = 0;
	onlyUnknownFileTypesFound = false;
	counterWriteUnknownFileTypes = 0;
	counterWriteOnlyUnknownFileTypes = 0;
	typesDeltaSum = 0;
}

//void ProcessHistory::addHistory(ProcessHistory history)
//{
//	counterWriteFile += history.counterWriteFile;
//	counterFileTypeChanged += history.counterFileTypeChanged;
//	entropyOfRead += history.entropyOfRead;
//	entropyOfWrite += history.entropyOfWrite;
//	counterDeleteFileW += history.counterDeleteFileW;
//	counterMoveFileWCounter += history.counterMoveFileWCounter;
//	counterCryptEncryptCounter += history.counterCryptEncryptCounter;
//	counterDisableShadowCopy += history.counterDisableShadowCopy;
//	counterDisableWindowsRestore += history.counterDisableWindowsRestore;
//	counterDisableWindowsDefender += history.counterDisableWindowsDefender;
//	counterWriteProcessMemoryCounter += history.counterWriteProcessMemoryCounter;
//	counterCreateRemoteThreadCounter += history.counterCreateRemoteThreadCounter;
//}


ProcessHistory::~ProcessHistory()
{
}

wstring ProcessHistory::GetTime(time_t& time_t_var)
{
	if (time_t_var > 0)
	{
		struct tm buf;
		wchar_t returnBuffer[30];
		localtime_s(&buf, &time_t_var);
		wcsftime(returnBuffer, 30, L"%d/%m/%y %X", &buf);
		return returnBuffer;
	}
	return L"";
}

wstring ProcessHistory::toString() {

	std::time_t ttp = std::chrono::system_clock::to_time_t(this->processCreationTime);

	double entropyAvgRead = this->counterReadFile != 0 ? this->entropyOfRead / this->counterReadFile : 0;
	double entropyAvgWrite = this->counterWriteFile != 0 ? this->entropyOfWrite / this->counterWriteFile : 0;
	double entropyAvgDelta = entropyAvgWrite - entropyAvgRead;

	return L"History STATs:\n" +
		wstring(L"counterDeleteFileW: ")			+ std::to_wstring(this->counterDeleteFileW) + L"\n" +
		wstring(L"counterDisableBackup: ")			+ std::to_wstring(this->counterDisableBackup) + L"\n" +
		wstring(L"counterDisableShadowCopy: ")		+ std::to_wstring(this->counterDisableShadowCopy) + L"\n" +
		wstring(L"counterDisableWindowsDefender: ") + std::to_wstring(this->counterDisableWindowsDefender) + L"\n" +
		wstring(L"counterDisableWindowsRestore: ")	+ std::to_wstring(this->counterDisableWindowsRestore) + L"\n" +
		wstring(L"counterFileTypeChanged: ")		+ std::to_wstring(this->counterFileTypeChanged) + L"\n" +
		wstring(L"counterInjectionCounter: ")		+ std::to_wstring(this->counterInjectionCounter) + L"\n" +
		wstring(L"counterMoveFileWCounter: ")		+ std::to_wstring(this->counterMoveFileWCounter) + L"\n" +
		wstring(L"counterReadFile: ")				+ std::to_wstring(this->counterReadFile) + L"\n" +
		wstring(L"counterWriteFile: ")				+ std::to_wstring(this->counterWriteFile) + L"\n" +
		wstring(L"counterWriteUnknownFileTypes: ")	+ std::to_wstring(this->counterWriteUnknownFileTypes) + L"\n" +
		wstring(L"onlyUnknownFileTypesFound: ")		+ (this->onlyUnknownFileTypesFound ? wstring(L"TRUE") : wstring(L"FALSE")) + L"\n" +
		wstring(L"counterWriteOnlyUnknownFileTypes: ") + std::to_wstring(this->counterWriteOnlyUnknownFileTypes) + L"\n" +
		wstring(L"entropyOfRead: ")					+ std::to_wstring(this->entropyOfRead) + L"\n" +
		wstring(L"entropyOfWrite: ")				+ std::to_wstring(this->entropyOfWrite) + L"\n" +	
		wstring(L"entropy Delta: ")					+ std::to_wstring(this->entropyOfWrite - this->entropyOfRead) + L"\n" +
		wstring(L"entropy Avg Delta: ")				+ std::to_wstring((this->entropyOfWrite / this->counterWriteFile) - (this->entropyOfRead / this->counterReadFile)) + L"\n" +
		wstring(L"entropy Sum Delta: ")				+ std::to_wstring(this->entropySumDelta) + L"\n" +
		wstring(L"listTypesRead.size: ")			+ std::to_wstring(this->listTypesRead.size()) + L"\n" +
		wstring(L"listTypesWrite.size: ")			+ std::to_wstring(this->listTypesWrite.size()) + L"\n" +
		wstring(L"listTypes.size Delta: ")			+ std::to_wstring(this->listTypesRead.size() - this->listTypesWrite.size()) + L"\n" +
		wstring(L"listTypes.size Sum Delta: ")		+ std::to_wstring(this->typesDeltaSum) + L"\n" +
		
		wstring(L"timeDiff: ")						+ std::to_wstring(this->timeDiff) + L"\n" +
		wstring(L"origin TimeStamp: ")				+ GetTime(ttp) + L"\n";
		
}