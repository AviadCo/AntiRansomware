#pragma once

#include "ProcessHistory.h"
#include "ProcessEvent.h"
#include "Logger.h"

#include <exception>
#include <Windows.h>
#include <inttypes.h>

using std::exception;

class ProcessPolicy
{
public:

	static const uint64_t SCORE_THRESHOLD = 300;

	//Scoring constants
	static const unsigned int FILE_DELETE_SCORE = 5;
	static const unsigned int FILE_RENAME_SCORE = 5;
	static const unsigned int FILE_WRITE_SCORE = 1;
	static const unsigned int FILE_WRITE_UNKNOWN_TYPE_SCORE = 1;
	static const unsigned int FILE_WRITE_ONLY_UNKNOWN_TYPES_SCORE = 1.5 * FILE_WRITE_UNKNOWN_TYPE_SCORE;
	static const unsigned int FILE_WRITE_HONEYPOT_SCORE = 5 * FILE_WRITE_SCORE;
	static const unsigned int FILE_READ_HONEYPOT_SCORE = 10 * FILE_WRITE_SCORE;
	static const unsigned int FILE_CHNAGE_TYPE_SCORE = 50;
	static const unsigned int ENCRYPTION_SCORE = 20;
	static const unsigned int PROCESS_INJECTION_SCORE = 40;
	static const unsigned int DISABLE_SHADOW_COPY_SCORE = 100;
	static const unsigned int DISABLE_WINDOWS_BACKUP_SCORE = 100;
	static const unsigned int DISABLE_WINDOWS_RESTORE_SCORE = 100;
	static const unsigned int DISABLOE_WINDOWS_DEFENDER_SCORE = 20;
	static const unsigned int CREATE_REMOTE_THREAD_SCORE = 10;
	
	
	static const double TIME_DURATION_RATIO;

	//Types Constants

	static const wstring FILE_TYPE_EMPTY;
	static const wstring FILE_TYPE_DATA;

	//Policy Thresholds
	static const unsigned int THRESHOLD_MINIMUM_ENTROPY_GROUP_SIZE = 50;
	static const unsigned int THRESHOLD_TOO_FEW_FILE_TYPES = 2;


	static unsigned int getScoreForOperation(ProcessEvent::ProcessOperationTag operation);
	static uint64_t getScoreForHistory(ProcessHistory& history, ProcessEvent& action);

private:
	static void updateEntropyAndTypes(ProcessHistory& history, ProcessEvent& action, set<wstring>& typesList, double& entropy);
	static void updateData(ProcessHistory& history, ProcessEvent& action);
	static void fixTimeStamp(std::chrono::time_point<std::chrono::system_clock>& timestamp, int secondsToAdd);
};

class ProcessPolicyException : public exception {};