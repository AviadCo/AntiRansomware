#pragma once
#include <Windows.h>
namespace FunctionHooksDefinitions {
	//TODO define a better splitter
	LPUWSTR PARAM_SPLITTER = L"|";

	namespace HookCreateFileW {
		LPUWSTR name = L"CreateFileW";

		typedef enum args { 
			FILEPATH		//char* 
		} args;
	}

	namespace HookWriteFile {
		LPUWSTR name = L"WriteFile";

		typedef enum args {
			FILEPATH		//char* 
		} args;
	}

	namespace HookDeleteFileW {
		LPUWSTR name = L"DeleteFileW";

		typedef enum args {
			FILEPATH		//char* 
		} args;
	}

	namespace HookMoveFileW {
		LPUWSTR name = L"MoveFileW";

		typedef enum args {
			FILEPATH_SRC,	//char*
			FILEPATH_DST	//char*
		} args;
	}

	namespace HookCryptEncrypt {
		LPUWSTR name = L"CryptEncrypt";
	}

	namespace HookShellExecuteEx {
		LPUWSTR name = L"ShellExecuteEx";

		typedef enum args {
			FILE_EXE,	//char*
			FILE_PARAM,	//char*
		} args;
	}

	namespace HookWriteProcessMemory {
		LPUWSTR name = L"WriteProcessMemory";

		typedef enum args {
			DESC_PID,	//char*
		} args;
	}

	namespace HookCreateProcess {
		LPUWSTR name = L"CreateProcess";

		typedef enum args {
			PID,	//char*
		} args;
	}
}
