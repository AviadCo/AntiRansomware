#pragma once

namespace FunctionHooksDefinitions {
	namespace HookCreateFileW {
		char* name = "CreateFileW";

		typedef enum args { 
			FILEPATH		//char* 
		} args;
	}

	namespace HookWriteFile {
		char* name = "WriteFile";

		typedef enum args {
			FILEPATH		//char* 
		} args;
	}

	namespace HookDeleteFileW {
		char* name = "DeleteFileW";

		typedef enum args {
			FILEPATH		//char* 
		} args;
	}

	namespace HookMoveFileW {
		char* name = "MoveFileW";

		typedef enum args {
			FILEPATH_SRC,	//char*
			FILEPATH_DST	//char*
		} args;
	}

	namespace HookCryptEncrypt {
		char* name = "CryptEncrypt";
	}
}
