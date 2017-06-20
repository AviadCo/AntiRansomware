#pragma once

#include <inttypes.h>
#include <chrono>
#include <ctime>

class ProcessHistory
{
public:
	std::chrono::time_point<std::chrono::system_clock> processCreationTime;

	uint64_t counterWriteFile;
	uint64_t counterFileTypeChanged;
	double entropyOfRead;
	double entropyOfWrite;
	uint64_t counterDeleteFileW;
	uint64_t counterMoveFileWCounter;
	uint64_t counterCryptEncryptCounter;
	uint64_t counterDisableShadowCopy;
	uint64_t counterDisableBackup;
	uint64_t counterDisableWindowsRestore;
	uint64_t counterDisableWindowsDefender;
	uint64_t counterWriteProcessMemoryCounter;
	uint64_t counterCreateRemoteThreadCounter;

	ProcessHistory();
	void addHistory(ProcessHistory history);
	~ProcessHistory();
};

