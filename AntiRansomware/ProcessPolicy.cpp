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

	default:
		throw ProcessPolicyException();
	}
}