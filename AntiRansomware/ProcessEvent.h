#pragma once

#include <string>

using std::wstring;

class ProcessEvent
{
public:

	typedef enum ProcessOperationTag {
		FILE_DELETE,
		FILE_RENAME,
		FILE_WRITE,
		FILE_READ,
		ENCRYPTION,
		PROCESS_INJECTION,
		DISABLE_SHADOW_COPY,
		DISABLE_WINDOWS_BACKUP,
		DISABLE_WINDOWS_RESTORE,
		DISABLOE_WINDOWS_DEFENDER,
		CREATE_PROCESS,
		INJECTION,
	} ProcessOperationTag;


	ProcessOperationTag tag;

	ProcessEvent(ProcessOperationTag tag);

protected:
	bool isStringEmpty(const wstring& str);
	double wstringToDouble(const wstring& str, bool& validFlag);
	int wstringToInt(const wstring& str, bool& validFlag);
	bool wstringToBool(const wstring& str, bool& validFlag);

};


