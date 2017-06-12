#pragma once

#include <exception>

using std::exception;

class ProcessPolicy
{
public:
	static const unsigned int FILE_DELETE_SCORE = 100;
	static const unsigned int FILE_RENAME_SCORE = 100;
	static const unsigned int FILE_CHNAGE_CONTENT_SCORE = 100;
	static const unsigned int ENCRYPTION_SCORE = 20;
	static const unsigned int PROCESS_INJECTION_SCORE = 10;
	static const unsigned int DISABLE_SHADOW_COPY_SCORE = 100;
	static const unsigned int DISABLE_WINDOWS_BACKUP_SCORE = 100;
	static const unsigned int DISABLE_WINDOWS_RESTORE_SCORE = 100;
	static const unsigned int DISABLOE_WINDOWS_DEFENDER_SCORE = 100;


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
	};

	static unsigned int getScoreForOperation(ProcessOperation operation);
};

class ProcessPolicyException : public exception {};