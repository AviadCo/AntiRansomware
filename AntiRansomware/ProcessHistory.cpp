#include "stdafx.h"
#include "ProcessHistory.h"


ProcessHistory::ProcessHistory()
{
	processCreationTime = std::chrono::system_clock::now();
	counterWriteFile = 0;
	counterFileTypeChanged = 0;
	entropyOfRead = 0;
	entropyOfWrite = 0;
	counterDeleteFileW = 0;
	counterMoveFileWCounter = 0;
	counterCryptEncryptCounter = 0;
	counterDisableShadowCopy = 0;
	counterDisableWindowsRestore = 0;
	counterDisableWindowsDefender = 0;
	counterWriteProcessMemoryCounter = 0;
	counterCreateRemoteThreadCounter = 0;
}

void ProcessHistory::addHistory(ProcessHistory history)
{
	counterWriteFile += history.counterWriteFile;
	counterFileTypeChanged += history.counterFileTypeChanged;
	entropyOfRead += history.entropyOfRead;
	entropyOfWrite += history.entropyOfWrite;
	counterDeleteFileW += history.counterDeleteFileW;
	counterMoveFileWCounter += history.counterMoveFileWCounter;
	counterCryptEncryptCounter += history.counterCryptEncryptCounter;
	counterDisableShadowCopy += history.counterDisableShadowCopy;
	counterDisableWindowsRestore += history.counterDisableWindowsRestore;
	counterDisableWindowsDefender += history.counterDisableWindowsDefender;
	counterWriteProcessMemoryCounter += history.counterWriteProcessMemoryCounter;
	counterCreateRemoteThreadCounter += history.counterCreateRemoteThreadCounter;
}


ProcessHistory::~ProcessHistory()
{
}
