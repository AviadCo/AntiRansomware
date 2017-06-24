#include "stdafx.h"

#include "Honeypot.h"
#include "Logger.h"
#include "HoneypotNameGenerator.h"
#include <string>

using namespace std;

/* This class handles the Honeypot file status and maintainers */
Honeypot::Honeypot(LPCWSTR lpFileName): lpFileName(lpFileName),
							priority(HoneypotNameGenerator::getFilePriority(lpFileName)) {
}

/************************************************************************/
/* This is Constructor                                                  */
/************************************************************************/
Honeypot::Honeypot(const wstring lpFileName) : lpFileName(lpFileName),
						 priority(HoneypotNameGenerator::getFilePriority(lpFileName)){
}

const wstring Honeypot::getFileName() const {
	return lpFileName;
}

//TODO remove this function
static wstring ExePath() {
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = wstring(buffer).find_last_of(L"\\/");
	return wstring(buffer).substr(0, pos);
}

DWORD Honeypot::create(bool listAccessTime) {
	wstring firstFile, lastFile, fileType;
	wstring dirPath = HoneypotNameGenerator::getFileDirectory(lpFileName);
	FILETIME accessTime, writeTime;

	fileType = HoneypotNameGenerator::getFileExtenstion(lpFileName);
	fileType = ExePath() + L"\\..\\fileTypes\\" + fileType + L"." + fileType;

	if (CopyFile(fileType.c_str(), lpFileName.c_str(), TRUE) == 0) {
		log().error(__FUNCTION__, L"CopyFile failed to create honeypot " + lpFileName + L", errno: " + to_wstring(GetLastError()));

		return GetLastError();
	}

	FileSystemHelper::getFirstAndLastAccessTimeOrder(dirPath, firstFile, lastFile);
	if (listAccessTime) {
		accessTime = FileSystemHelper::getFileAttribute(dirPath + L"\\" + firstFile, FileSystemHelper::FILE_ACCESS_TIME_ATTRIBUTE);
		accessTime.dwLowDateTime -= 5;
		accessTime.dwHighDateTime -= 5;
	}
	else {
		accessTime = FileSystemHelper::getFileAttribute(dirPath + L"\\" + lastFile, FileSystemHelper::FILE_ACCESS_TIME_ATTRIBUTE);
		accessTime.dwLowDateTime += 5;
		accessTime.dwHighDateTime += 5;
	}

	FileSystemHelper::getFirstAndLastFileWriteTimeOrder(dirPath, firstFile, lastFile);
	if (listAccessTime) {
		writeTime = FileSystemHelper::getFileAttribute(dirPath + L"\\" + firstFile, FileSystemHelper::FILE_WRITE_TIME_ATTRIBUTE);
		writeTime.dwLowDateTime -= 5;
		writeTime.dwHighDateTime -= 5;
	}
	else {
		writeTime = FileSystemHelper::getFileAttribute(dirPath + L"\\" + lastFile, FileSystemHelper::FILE_WRITE_TIME_ATTRIBUTE);
		writeTime.dwLowDateTime += 5;
		writeTime.dwHighDateTime += 5;
	}

	FileSystemHelper::setFileAttribute(lpFileName, FileSystemHelper::getFileAttribute(lpFileName, FileSystemHelper::FILE_CREATION_TIME_ATTRIBUTE),
									   accessTime, writeTime);

	return 0;
}

#define TEMP_BUFFER_LENGTH 10000
bool Honeypot::compareFile(FILE* f1, FILE* f2) const {
	char buf1[TEMP_BUFFER_LENGTH];
	char buf2[TEMP_BUFFER_LENGTH];

	do {
		size_t r1 = fread(buf1, 1, TEMP_BUFFER_LENGTH, f1);
		size_t r2 = fread(buf2, 1, TEMP_BUFFER_LENGTH, f2);

		if (r1 != r2 || memcmp(buf1, buf2, r1)) {
			return false;
		}
	} while (!feof(f1) || !feof(f2));

	return feof(f1) && feof(f2);
}

bool Honeypot::isChanged() const {
	bool result;
	errno_t err;
	wstring fileType;
	FILE *originalFile, *honeypotFile;

	fileType = HoneypotNameGenerator::getFileExtenstion(lpFileName);
	fileType = ExePath() + L"\\..\\fileTypes\\" + fileType + L"." + fileType;

	err = fopen_s(&originalFile, string(fileType.begin(), fileType.end()).c_str(), "r");
	if (err != 0) {

		log().error(__FUNCTION__, L"fopen_s failed to open file " + fileType + L" errno: " + to_wstring(err));

		return true;
	}
	err = fopen_s(&honeypotFile, string(lpFileName.begin(), lpFileName.end()).c_str(), "r");
	if (err != 0) {

		log().error(__FUNCTION__, L"fopen_s failed to open file " + lpFileName + to_wstring(err));

		return true;
	}

	result = !compareFile(originalFile, honeypotFile);

	fclose(honeypotFile);
	fclose(originalFile);

	return result;
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

int Honeypot::getPriority() const
{
	return priority;
}
