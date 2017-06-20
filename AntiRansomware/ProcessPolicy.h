#pragma once

#include <exception>
#include "ProcessHistory.h"
#include <inttypes.h>

using std::exception;

class ProcessPolicy
{
public:
	static const unsigned int FILE_DELETE_SCORE = 10;
	static const unsigned int FILE_RENAME_SCORE = 10;
	static const unsigned int FILE_CHNAGE_CONTENT_SCORE = 2;
	static const unsigned int FILE_CHNAGE_TYPE_SCORE = 50;
	static const unsigned int ENCRYPTION_SCORE = 20;
	static const unsigned int PROCESS_INJECTION_SCORE = 20;
	static const unsigned int DISABLE_SHADOW_COPY_SCORE = 100;
	static const unsigned int DISABLE_WINDOWS_BACKUP_SCORE = 100;
	static const unsigned int DISABLE_WINDOWS_RESTORE_SCORE = 100;
	static const unsigned int DISABLOE_WINDOWS_DEFENDER_SCORE = 20;
	static const unsigned int CREATE_REMOTE_THREAD_SCORE = 10;


	enum ProcessOperation {
		FILE_DELETE,
		FILE_RENAME,
		FILE_CHNAGE_CONTENT,
		ENCRYPTION,
		PROCESS_INJECTION,
		DISABLE_SHADOW_COPY,
		DISABLE_WINDOWS_BACKUP,
		DISABLE_WINDOWS_RESTORE,
		DISABLOE_WINDOWS_DEFENDER,
		CREATE_PROCESS,
		CREATE_REMOTE_THREAD,
	};

	static unsigned int getScoreForOperation(ProcessOperation operation);
	static uint64_t getScoreForHistory(ProcessHistory history);
};

class ProcessPolicyException : public exception {};