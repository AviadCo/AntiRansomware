#pragma once
#include <inttypes.h>
class ProcessHistory
{
public:
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

