#include "stdafx.h"
#include "ProcessPolicy.h"
#include "ProcessFileIOEvent.h"

#include <cmath>

/*******************************
	constants definitions
*******************************/
const wstring ProcessPolicy::FILE_TYPE_EMPTY = L"empty";
const wstring ProcessPolicy::FILE_TYPE_DATA = L"data";

const double ProcessPolicy::TIME_DURATION_RATIO = 1;

unsigned int ProcessPolicy::getScoreForOperation(ProcessEvent::ProcessOperationTag operation)
{
	switch (operation) {
	case ProcessEvent::FILE_DELETE:
		return FILE_DELETE_SCORE;

	case ProcessEvent::FILE_RENAME:
		return FILE_RENAME_SCORE;

	case ProcessEvent::FILE_WRITE:
		return FILE_WRITE_SCORE;

	case ProcessEvent::ENCRYPTION:
		return ENCRYPTION_SCORE;

	case ProcessEvent::PROCESS_INJECTION:
		return PROCESS_INJECTION_SCORE;

	case ProcessEvent::DISABLE_SHADOW_COPY:
		return DISABLE_SHADOW_COPY_SCORE;

	case ProcessEvent::DISABLE_WINDOWS_BACKUP:
		return DISABLE_WINDOWS_BACKUP_SCORE;

	case ProcessEvent::DISABLE_WINDOWS_RESTORE:
		return DISABLE_WINDOWS_RESTORE_SCORE;

	case ProcessEvent::DISABLOE_WINDOWS_DEFENDER:
		return DISABLOE_WINDOWS_DEFENDER_SCORE;

	case ProcessEvent::INJECTION:
		return CREATE_REMOTE_THREAD_SCORE;

	default:
		throw ProcessPolicyException();
	}
}

uint64_t ProcessPolicy::getScoreForHistory(ProcessHistory& history, ProcessEvent& action)
{
	updateData(history, action);

	uint64_t score = 0;

	score += history.counterInjectionCounter * ProcessPolicy::PROCESS_INJECTION_SCORE;
	score += history.counterDisableBackup * ProcessPolicy::DISABLE_WINDOWS_BACKUP_SCORE;
	score += history.counterDisableShadowCopy * ProcessPolicy::DISABLE_WINDOWS_BACKUP_SCORE;
	score += history.counterDisableWindowsDefender * ProcessPolicy::DISABLOE_WINDOWS_DEFENDER_SCORE;
	score += history.counterDisableWindowsRestore * ProcessPolicy::DISABLE_WINDOWS_RESTORE_SCORE;
	score += history.counterWriteFile * ProcessPolicy::FILE_WRITE_SCORE;
	score += history.counterFileTypeChanged * ProcessPolicy::FILE_CHNAGE_TYPE_SCORE;
	score += history.counterDeleteFileW * ProcessPolicy::FILE_DELETE_SCORE;
	score += history.counterCryptEncryptCounter * ProcessPolicy::ENCRYPTION_SCORE;
	score += history.counterMoveFileWCounter * ProcessPolicy::FILE_RENAME_SCORE;
	


	score += history.entropySumDelta;


	score += history.typesDeltaSum;

	score += history.counterWriteUnknownFileTypes * ProcessPolicy::FILE_WRITE_UNKNOWN_TYPE_SCORE;

	score += history.counterWriteOnlyUnknownFileTypes * ProcessPolicy::FILE_WRITE_ONLY_UNKNOWN_TYPES_SCORE;

	
	uint64_t discountPoints = history.timeDiff * TIME_DURATION_RATIO;

	log().debug(__FUNCTION__, L"bad points: " + std::to_wstring(score) + L" good points: " + std::to_wstring(discountPoints));

	if (discountPoints <= score) {
		score -= discountPoints;
	}
	else {
		uint64_t pointsDiff = discountPoints -  score;
		fixTimeStamp(history.processCreationTime, pointsDiff / TIME_DURATION_RATIO);
		score = 0;
	}

	return score;

}

void ProcessPolicy::updateData(ProcessHistory& history, ProcessEvent& action) {

	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - history.processCreationTime;

	history.timeDiff = elapsed_seconds.count();

	switch (action.tag)
	{
	case ProcessEvent::CREATE_PROCESS:
		break;
	case ProcessEvent::INJECTION:
		history.counterInjectionCounter++;
		break;
	case ProcessEvent::DISABLE_SHADOW_COPY:
		history.counterDisableShadowCopy++;
		break;
	case ProcessEvent::DISABLE_WINDOWS_BACKUP:
		history.counterDisableBackup++;
		break;
	case ProcessEvent::DISABLE_WINDOWS_RESTORE:
		history.counterDisableWindowsRestore++;
		break;
	case ProcessEvent::DISABLOE_WINDOWS_DEFENDER:
		history.counterDisableWindowsDefender++;
		break;
	case ProcessEvent::ENCRYPTION:
		history.counterCryptEncryptCounter++;
		break;
	case ProcessEvent::FILE_DELETE:
		history.counterDeleteFileW++;
		break;

	case ProcessEvent::FILE_RENAME:
		history.counterMoveFileWCounter++;
		break;
	case ProcessEvent::FILE_READ:
		history.counterReadFile++;
		updateEntropyAndTypes(history, action, history.listTypesRead, history.entropyOfRead);
		break;
	case ProcessEvent::FILE_WRITE:
		updateEntropyAndTypes(history, action, history.listTypesWrite, history.entropyOfWrite);
		history.counterWriteFile++;
		break;
	}
}

void ProcessPolicy::updateEntropyAndTypes(ProcessHistory& history, ProcessEvent& action, set<wstring>& typesList, double& entropy) {

	ProcessFileIOEvent& ioAction = static_cast<ProcessFileIOEvent&>(action);

	if (ioAction.entropyValid && ioAction.entropyGroupSizeValid) {
		if (ioAction.entropyGroupSize > THRESHOLD_MINIMUM_ENTROPY_GROUP_SIZE) {
			entropy += ioAction.entropy;
		}
	}

	if (ioAction.fileTypeValid) {
		typesList.insert(ioAction.fileType);
	}

	if (ioAction.isFileSameValid && !ioAction.isFileSame) {
		history.counterFileTypeChanged++;
	}


	//when writes - the process get penalty on its entropy delta, files type, and unknown types
	if (action.tag == ProcessEvent::FILE_WRITE) {

		double entropyAvgRead = history.counterReadFile != 0 ? history.entropyOfRead / history.counterReadFile : 0;
		double entropyAvgWrite = history.counterWriteFile != 0 ? history.entropyOfWrite / history.counterWriteFile : 0;
		double entropyAvgDelta = entropyAvgWrite - entropyAvgRead;

		if (entropyAvgDelta > 0) {
			history.entropySumDelta += ceil(entropyAvgDelta);
		}


		int typesCountDelta = history.listTypesRead.size() - history.listTypesWrite.size();

		if (typesCountDelta > 0) {
			history.typesDeltaSum += typesCountDelta;
		}


		//check if process write too few and unknown file types
		if (history.listTypesWrite.size() <= THRESHOLD_TOO_FEW_FILE_TYPES) {

			if (!history.onlyUnknownFileTypesFound) {
				//if not found yet - search for them in the set
				int filterTypesCount = 0;
				for each (wstring type in history.listTypesWrite)
				{
					if (type.compare(FILE_TYPE_DATA) == 0) {
						filterTypesCount++;
					}
				}

				if (filterTypesCount == 1) {
					history.onlyUnknownFileTypesFound = true;
				}
			}

		}
		else {
			history.onlyUnknownFileTypesFound = false;
		}

		//check if writing to unknown file
		if (ioAction.fileTypeValid) {
			if (ioAction.fileType.compare(FILE_TYPE_DATA) == 0) {

				if (history.onlyUnknownFileTypesFound) {
					history.counterWriteOnlyUnknownFileTypes++;
				}
				else {
					history.counterWriteUnknownFileTypes++;
				}
				
			}
		}

	}

}

void ProcessPolicy::fixTimeStamp(std::chrono::time_point<std::chrono::system_clock>& timestamp, int secondsToAdd) {
	timestamp += std::chrono::seconds(secondsToAdd);
}