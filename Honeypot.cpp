#include "stdafx.h"

#include "Honeypot.h"
#include "Logger.h"
#include <string>

using namespace std;

#define FILE_CONTENT L"@The is the original content of the Honeypot\n"

#define LENGTH_FILE_CONTENT (sizeof(FILE_CONTENT))

/* This class handles the Honeypot file status and maintainers */
Honeypot::Honeypot(LPCWSTR lpFileName): lpFileName(lpFileName){
}

/************************************************************************/
/* This is Constructor                                                  */
/************************************************************************/
Honeypot::Honeypot(const wstring lpFileName) : lpFileName(lpFileName) {
}

const wstring Honeypot::getFileName() {
	return lpFileName;
}

DWORD Honeypot::create() {
	HANDLE fileHandle = CreateFile(lpFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_NEW, FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		log().error(__FUNCTION__, L"CreateFile failed to open Honeypot " + lpFileName + L", errno: " + to_wstring(GetLastError()));

		return GetLastError();
	}

	if (!WriteFile(fileHandle, FILE_CONTENT, LENGTH_FILE_CONTENT, NULL, NULL)) {
		/* Write content to file failed */
		log().error(__FUNCTION__, L"CreateFile failed to write content to Honeypot " + lpFileName + L", errno: " + to_wstring(GetLastError()));
		log().error(__FUNCTION__, L"Removing Honeypot from system.");

		DeleteFile(lpFileName.c_str());

		return GetLastError();
	}

	CloseHandle(fileHandle);

	return 0;
}

bool Honeypot::isChanged() {
	wchar_t readContent[LENGTH_FILE_CONTENT + 1024];

	HANDLE fileHandle = CreateFile(lpFileName.c_str(), GENERIC_READ,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		/* File creation failed */
		log().error(__FUNCTION__, L"Failed to open Honeypot " + lpFileName + L" to check modifications.");

		return true;
	}

	if (!ReadFile(fileHandle, readContent, LENGTH_FILE_CONTENT, NULL, NULL)) {
		log().error(__FUNCTION__, L"Failed to read Honeypot " + lpFileName + L" to check modifications.");

		CloseHandle(fileHandle);

		return true;
	}

	CloseHandle(fileHandle);

	if (wcslen(FILE_CONTENT) != wcslen(readContent)) {
		log().info(__FUNCTION__, L"Honeypot " + lpFileName + L" content length was modified.");

		return true;
	}

	return !!wcsncmp(readContent, FILE_CONTENT, wcslen(FILE_CONTENT));
}

bool Honeypot::destroy() {
	return !!DeleteFile(lpFileName.c_str());
}

bool Honeypot::operator==(const Honeypot & other)
{
	return this->lpFileName == other.lpFileName;
}

bool Honeypot::operator==(const wstring & other)
{
	return this->lpFileName == other;
}

bool Honeypot::operator==(LPCWSTR other)
{
	return *this == wstring(other);
}
