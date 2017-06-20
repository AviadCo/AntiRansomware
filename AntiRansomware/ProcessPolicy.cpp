#include "stdafx.h"
#include "ProcessPolicy.h"

unsigned int ProcessPolicy::getScoreForOperation(ProcessOperation operation)
{
	switch (operation) {
	case FILE_DELETE:
		return FILE_DELETE_SCORE;

	case FILE_RENAME:
		return FILE_RENAME_SCORE;

	case FILE_CHNAGE_CONTENT:
		return FILE_CHNAGE_CONTENT_SCORE;

	case ENCRYPTION:
		return ENCRYPTION_SCORE;

	case PROCESS_INJECTION:
		return PROCESS_INJECTION_SCORE;

	case DISABLE_SHADOW_COPY:
		return DISABLE_SHADOW_COPY_SCORE;

	case DISABLE_WINDOWS_BACKUP:
		return DISABLE_WINDOWS_BACKUP_SCORE;

	case DISABLE_WINDOWS_RESTORE:
		return DISABLE_WINDOWS_RESTORE_SCORE;

	case DISABLOE_WINDOWS_DEFENDER:
		return DISABLOE_WINDOWS_DEFENDER_SCORE;

	case CREATE_REMOTE_THREAD:
		return CREATE_REMOTE_THREAD_SCORE;

	default:
		throw ProcessPolicyException();
	}
}

uint64_t ProcessPolicy::getScoreForHistory(ProcessHistory history)
{
	uint64_t score = 0;

	score += history.counterWriteProcessMemoryCounter * PROCESS_INJECTION;
	score += history.counterDisableBackup * DISABLE_WINDOWS_BACKUP;
	score += history.counterDisableShadowCopy * DISABLE_WINDOWS_BACKUP;
	score += history.counterDisableWindowsDefender * DISABLOE_WINDOWS_DEFENDER;
	score += history.counterDisableWindowsRestore * DISABLE_WINDOWS_RESTORE;
	score += history.counterWriteFile * FILE_CHNAGE_CONTENT_SCORE;
	score += history.counterFileTypeChanged * FILE_CHNAGE_TYPE_SCORE;
	score += history.counterDeleteFileW * FILE_DELETE_SCORE;
	score += history.counterCryptEncryptCounter * ENCRYPTION_SCORE;
	score += history.counterMoveFileWCounter * FILE_RENAME_SCORE;

	return score;

}
