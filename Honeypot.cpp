#include "stdafx.h"
#include "Honeypot.h"

#define FILE_CONTENT L"@NOAM YOU MAKE ME CRAZY AHHHHHHHHHHHHH 01234\n"

#define MAX_LENGTH_READ_FILE_CONTENT (sizeof(FILE_CONTENT) + 1024)

using namespace std;

/* This class handles the Honeyput file status and maintainess */
Honeypot::Honeypot(LPCTSTR lpFileName) {
	this->lpFileName = lpFileName;
}

LPCTSTR Honeypot::getFileName() {
	return lpFileName;
}

DWORD Honeypot::create() {
	HANDLE fileHandle = CreateFile(lpFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_NEW, FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		/* File creation failed */
		return GetLastError();
	}

	if (!WriteFile(fileHandle, FILE_CONTENT, wcslen(FILE_CONTENT), NULL, NULL)) {
		/* Write content to file failed */
		DeleteFile(lpFileName);

		return GetLastError();
	}

	CloseHandle(fileHandle);

	return 0;
}

bool Honeypot::isChanged() {
	wchar_t readContent[MAX_LENGTH_READ_FILE_CONTENT];

	HANDLE fileHandle = CreateFile(lpFileName, GENERIC_READ,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		/* File creation failed */
		return true;
	}

	if (!ReadFile(fileHandle, readContent, MAX_LENGTH_READ_FILE_CONTENT, NULL, NULL)) {
		CloseHandle(fileHandle);

		return true;
	}

	CloseHandle(fileHandle);

	if (wcslen(FILE_CONTENT) != wcslen(readContent)) {
		return true;
	}

	return !!wcsncmp(readContent, FILE_CONTENT, wcslen(FILE_CONTENT));
}

bool Honeypot::destory() {
	return !!DeleteFile(lpFileName);
}