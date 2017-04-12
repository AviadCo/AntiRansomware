#include <windows.h>
#include <string.h>

#include "stdafx.h"
#include "Honeypot.h"

#define FILE_CONTENT "@NOAM YOU MAKE ME CRAZY AHHHHHHHHHHHHH 01234\n"

#define MAX_LENGTH_READ_FILE_CONTENT (sizeof(FILE_CONTENT) + 1024)

/* This class handles the Honeyput file status and maintainess */
class Honeypot {
	private:
	LPCTSTR lpFileName;
	HANDLE fileHandle;

	public:
		Honeypot(LPCTSTR lpFileName)
		{
			this->lpFileName = lpFileName;
		}

		DWORD create() {
			fileHandle = CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL, CREATE_NEW, FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			
			if (fileHandle == INVALID_HANDLE_VALUE) {
				/* File creation failed */
				return GetLastError();
			}

			if (!WriteFile(fileHandle, FILE_CONTENT, sizeof(FILE_CONTENT), NULL, NULL)) {
				/* Write content to file failed */
				DeleteFile(lpFileName);

				return GetLastError();
			}

			return 0;
		}

		bool isChanged() {
			char readContent[MAX_LENGTH_READ_FILE_CONTENT];

			if (!ReadFile(fileHandle, readContent, MAX_LENGTH_READ_FILE_CONTENT, NULL, NULL)) {
				return true;
			}

			if (strlen(FILE_CONTENT) != strlen(readContent)) {
				return true;
			}

			return !strncmp(readContent, FILE_CONTENT, strlen(FILE_CONTENT));
		}

		void destory() {
			DeleteFile(lpFileName);
		}
};