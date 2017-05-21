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

	enum ProcessOperation {
		FILE_DELETE,
		FILE_RENAME,
		FILE_CHNAGE_CONTENT,
		ENCRYPTION,
	};

	static unsigned int getScoreForOperation(ProcessOperation operation);
};

class ProcessPolicyException : public exception {};