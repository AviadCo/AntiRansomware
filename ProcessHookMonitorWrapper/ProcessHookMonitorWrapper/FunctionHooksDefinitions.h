#pragma once
#include <Windows.h>
namespace FunctionHooksDefinitions {
	//TODO define a better splitter
	LPUWSTR PARAM_SPLITTER = L"|/|/|";

	namespace HookCreateFileW {
		LPUWSTR name = L"CreateFileW";

		typedef enum args { 
			FILEPATH		//char* 
		} args;
	}

	namespace HookWriteFile {
		LPUWSTR name = L"WriteFile";

		typedef enum args {
			FILEPATH,		//char* 
			FILE_TYPE,		//char* 
			IS_TYPE_SAME,	//int (1-true, 0-flase)
			ENTROPY,		//double 
			ENTROPY_SIZE,	//int
		} args;
	}

	namespace HookReadFile {
		LPUWSTR name = L"ReadFile";

		typedef enum args {
			FILEPATH,		//char* 
			FILE_TYPE,		//char* 
			ENTROPY,		//double 
			ENTROPY_SIZE,	//int 
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

	namespace HookShellExecuteExW {
		LPUWSTR name = L"ShellExecuteExW";

		typedef enum args {
			FILE_EXE,	//char*
			FILE_PARAM,	//char*
		} args;
	}

	namespace HookWriteProcessMemory {
		LPUWSTR name = L"WriteProcessMemory";

		typedef enum args {
			DESC_PID,	//int
		} args;
	}

	namespace HookCreateProcess {
		LPUWSTR name = L"CreateProcessW";

		typedef enum args {
			PID,	//int
			APPLICATION_NAME,	//char*
			COMMAND_LINE,	//char*
		} args;
	}

	namespace HookCreateRemoteThread {
		LPUWSTR name = L"CreateRemoteThread";

		typedef enum args {
			PID,	//int
		} args;
	}

	namespace HookCreateRemoteThreadEx {
		LPUWSTR name = L"CreateRemoteThreadEx";

		typedef enum args {
			PID,	//int
		} args;
	}
}
