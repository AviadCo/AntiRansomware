#pragma once

#include <inttypes.h>
#include <chrono>
#include <ctime>
#include <string>
#include <set>

using std::set;
using std::wstring;

class ProcessHistory
{
public:
	std::chrono::time_point<std::chrono::system_clock> processCreationTime;

	uint64_t counterWriteFile;
	uint64_t counterReadFile;
	uint64_t counterFileTypeChanged;
	double entropyOfRead;
	double entropyOfWrite;
	double entropySumDelta;
	uint64_t counterDeleteFileW;
	uint64_t counterMoveFileWCounter;
	uint64_t counterCryptEncryptCounter;
	uint64_t counterDisableShadowCopy;
	uint64_t counterDisableBackup;
	uint64_t counterDisableWindowsRestore;
	uint64_t counterDisableWindowsDefender;
	uint64_t counterInjectionCounter;
	uint64_t timeDiff;
	bool onlyUnknownFileTypesFound;
	uint64_t counterWriteUnknownFileTypes;
	uint64_t counterWriteOnlyUnknownFileTypes;
	
	set<wstring> listTypesRead;
	set<wstring> listTypesWrite;
	uint64_t typesDeltaSum;

	ProcessHistory();
	//void addHistory(ProcessHistory history);
	wstring toString();
	~ProcessHistory();

	wstring GetTime(time_t& time_t_var);
};

