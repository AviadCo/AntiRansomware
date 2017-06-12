#pragma once
#include <Windows.h>
namespace FunctionHooksDefinitions {
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

	namespace WriteProcessMemory {
		LPUWSTR name = L"WriteProcessMemory";

		typedef enum args {
			DESC_PID,	//char*
		} args;
	}
}
