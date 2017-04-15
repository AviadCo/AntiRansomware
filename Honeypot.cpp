#include "stdafx.h"
#include "Honeypot.h"
#include "Logger.h"

#define FILE_CONTENT L"@NOAM YOU MAKE ME CRAZY AHHHHHHHHHHHHH 01234\n"

#define MAX_LENGTH_READ_FILE_CONTENT ((sizeof(FILE_CONTENT) + 1024) * sizeof(wchar_t))

using namespace std;

void Honeypot::findUserDirectories()
{
	
	/*PWSTR strResult;
	HRESULT SHGetKnownFolderPath(	_In_     REFKNOWNFOLDERID rfid,
		0, NULL, &strResult
	);*/

}

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
		log().error(__FUNCTION__, "CreateFile failed to open Honeypot " + Logger::unicodeToString(lpFileName) + ", errno: " + to_string(GetLastError()));

		return GetLastError();
	}

	if (!WriteFile(fileHandle, FILE_CONTENT, wcslen(FILE_CONTENT), NULL, NULL)) {
		/* Write content to file failed */
		log().error(__FUNCTION__, "CreateFile failed to write content to Honeyput " + Logger::unicodeToString(lpFileName) + ", errno: " + to_string(GetLastError()));
		log().error(__FUNCTION__, "Removing Honeypot from system.");

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
		log().error(__FUNCTION__, "Failed to open Honeypot " + Logger::unicodeToString(lpFileName) + " to check modifications.");

		return true;
	}

	if (!ReadFile(fileHandle, readContent, MAX_LENGTH_READ_FILE_CONTENT, NULL, NULL)) {
		log().error(__FUNCTION__, "Failed to read Honeypot " + Logger::unicodeToString(lpFileName) + " to check modifications.");

		CloseHandle(fileHandle);

		return true;
	}

	CloseHandle(fileHandle);

	if (wcslen(FILE_CONTENT) != wcslen(readContent)) {
		log().info(__FUNCTION__, "Honeypot " + Logger::unicodeToString(lpFileName) + " content length was modified.");

		return true;
	}

	return !!wcsncmp(readContent, FILE_CONTENT, wcslen(FILE_CONTENT));
}

bool Honeypot::destroy() {
	return !!DeleteFile(lpFileName);
}