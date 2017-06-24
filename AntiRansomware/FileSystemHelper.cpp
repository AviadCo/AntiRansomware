#include "stdafx.h"

#include "FileSystemHelper.h"
#include "Logger.h"

#include <shlobj.h>

using namespace std;
using std::function;

list<wstring> FileSystemHelper::getAllSubDirectories(const wstring& directoryPath)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	list<wstring> subDirectories = list<wstring>();

	log().debug(__FUNCTION__, L"Getting Sub-Directories of " + directoryPath);

	if (directoryPath.length() > MAX_PATH - 3) {
		log().error(__FUNCTION__, directoryPath + L" length is greater than my max path allowed");

		throw FileSystemHelperException();
	}

	/* Prepare string for use with FindFile functions.  First, copy the
	   string to a buffer, then append '\*' to the directory name. */

	hFind = FindFirstFile((directoryPath + L"\\*").c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		log().error(__FUNCTION__, L"Failed to get first file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		throw FileSystemHelperException();
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!wcscmp(ffd.cFileName, L".") || (!wcscmp(ffd.cFileName, L".."))) {
				continue;
			}
			wstring subDir = wstring(directoryPath + L"\\" + ffd.cFileName);
			list<wstring> subDirList = list<wstring>();

			log().debug(__FUNCTION__, L"Found sub directory " + subDir + L" in " + directoryPath);
			
			subDirectories.push_back(subDir);
			subDirList = getAllSubDirectories(subDir);
			subDirectories.insert(subDirectories.end(), subDirList.begin(), subDirList.end());
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		log().error(__FUNCTION__, L"Failed to get new file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		FindClose(hFind);

		throw FileSystemHelperException();
	}

	FindClose(hFind);
	return subDirectories;
}

list<wstring> FileSystemHelper::getAllFilesInDir(const wstring& directoryPath)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	list<wstring> fileNames = list<wstring>();

	log().debug(__FUNCTION__, L"Getting file names in " + directoryPath);

	if (directoryPath.length() > MAX_PATH - 3) {
		log().error(__FUNCTION__, directoryPath + L" length is greater than my max path allowed");

		throw FileSystemHelperException();
	}

	/* Prepare string for use with FindFile functions.  First, copy the
	string to a buffer, then append '\*' to the directory name. */

	hFind = FindFirstFile((directoryPath + L"\\*").c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		log().error(__FUNCTION__, L"Failed to get first file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		throw FileSystemHelperException();
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else {
			fileNames.push_back(ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		log().error(__FUNCTION__, L"Failed to get new file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		FindClose(hFind);

		throw FileSystemHelperException();
	}

	FindClose(hFind);
	return fileNames;
}

bool compareFilesByAccessTime(const WIN32_FIND_DATA & a, const WIN32_FIND_DATA & b)
{
	return (CompareFileTime(&a.ftLastAccessTime, &b.ftLastAccessTime) < 0) ? true : false;
}

bool compareFilesLexicographic(const WIN32_FIND_DATA & a, const WIN32_FIND_DATA & b)
{
	return a.cFileName < b.cFileName;
}

bool compareFilesByWriteTime(const WIN32_FIND_DATA & a, const WIN32_FIND_DATA & b)
{
	return (CompareFileTime(&a.ftLastWriteTime, &b.ftLastWriteTime) < 0) ? true : false;
}

void FileSystemHelper::getFirstAndLastByCoparable(const wstring & directoryPath, wstring & firstFile, wstring & lastFile, std::function<bool(const WIN32_FIND_DATA &, const WIN32_FIND_DATA &)> compare)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	list<WIN32_FIND_DATA> filesData = list<WIN32_FIND_DATA>();

	log().debug(__FUNCTION__, L"Getting first and last file name in " + directoryPath + L" according to access time order");

	if (directoryPath.length() > MAX_PATH - 3) {
		log().error(__FUNCTION__, directoryPath + L" length is greater than my max path allowed");

		throw FileSystemHelperException();
	}

	/* Prepare string for use with FindFile functions.  First, copy the
	string to a buffer, then append '\*' to the directory name. */

	hFind = FindFirstFile((directoryPath + L"\\*").c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		log().error(__FUNCTION__, L"Failed to get first file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		throw FileSystemHelperException();
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else {
			filesData.push_back(ffd);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		log().error(__FUNCTION__, L"Failed to get new file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		FindClose(hFind);

		throw FileSystemHelperException();
	}

	FindClose(hFind);

	filesData.sort(compare);

	firstFile = filesData.front().cFileName;
	lastFile = filesData.back().cFileName;
}


void FileSystemHelper::getFirstAndLastAccessTimeOrder(const wstring& directoryPath, wstring& firstFile, wstring& lastFile)
{
	log().debug(__FUNCTION__, L"Getting first and last file name in " + directoryPath + L" according to access time order");

	getFirstAndLastByCoparable(directoryPath, firstFile, lastFile, compareFilesByAccessTime);
}

void FileSystemHelper::getFirstAndLastFileLexicographicOrder(const wstring& directoryPath, wstring& firstFile, wstring& lastFile)
{
	log().debug(__FUNCTION__, L"Getting first and last file name in " + directoryPath + L" according to lexicographic order");

	getFirstAndLastByCoparable(directoryPath, firstFile, lastFile, compareFilesLexicographic);
}

void FileSystemHelper::getFirstAndLastFileWriteTimeOrder(const wstring& directoryPath, wstring& firstFile, wstring& lastFile)
{
	log().debug(__FUNCTION__, L"Getting first and last file name in " + directoryPath + L" according to write time order");

	getFirstAndLastByCoparable(directoryPath, firstFile, lastFile, compareFilesByWriteTime);
}

FILETIME FileSystemHelper::getFileAttribute(const wstring & filename, int fileAttributeType)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	log().debug(__FUNCTION__, L"Getting file name in " + filename);

	if (filename.length() > MAX_PATH - 3) {
		log().error(__FUNCTION__, filename + L" length is greater than my max path allowed");

		throw FileSystemHelperException();
	}

	hFind = FindFirstFile((filename).c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		log().error(__FUNCTION__, L"Failed to get file in " + filename + L"Error: " + to_wstring(GetLastError()));

		throw FileSystemHelperException();
	}

	FindClose(hFind);

	switch (fileAttributeType) {
	case FILE_ACCESS_TIME_ATTRIBUTE:
		return ffd.ftLastAccessTime;

	case FILE_CREATION_TIME_ATTRIBUTE:
		return ffd.ftCreationTime;

	case FILE_WRITE_TIME_ATTRIBUTE:
		return ffd.ftLastWriteTime;

	default:
		log().error(__FUNCTION__, L"Failed to get file attribute with type " + to_wstring(fileAttributeType) + L" for file " + filename);

		throw FileSystemHelperException();
	}
}

void FileSystemHelper::setFileAttribute(const wstring & filename, FILETIME creationTime, FILETIME accessTime, FILETIME writeTime)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	log().debug(__FUNCTION__, L"Getting file name in " + filename);

	if (filename.length() > MAX_PATH - 3) {
		log().error(__FUNCTION__, filename + L" length is greater than my max path allowed");

		throw FileSystemHelperException();
	}

	hFile = CreateFile((filename).c_str(), // file to be opened
		GENERIC_WRITE, // open for writing
		FILE_SHARE_WRITE, // share for writing
		NULL,
		OPEN_ALWAYS,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		log().error(__FUNCTION__, L"Failed to get file in " + filename + L"Error: " + to_wstring(GetLastError()));

		throw FileSystemHelperException();
	}

	if (SetFileTime(hFile, &creationTime, &accessTime, &writeTime) == 0) {
		log().error(__FUNCTION__, L"Failed to set file attributes to " + filename + L"Error: " + to_wstring(GetLastError()));

		CloseHandle(hFile);

		throw FileSystemHelperException();
	}

	CloseHandle(hFile);
}

bool FileSystemHelper::isTempOrAppData(const wstring & filename)
{
	wstring TempPath;
	wchar_t wcharPath[MAX_PATH];

	/* Checking if file is temp file */
	if (GetTempPathW(MAX_PATH, wcharPath)) {
		TempPath = wcharPath;
	}
	else {
		log().error(__FUNCTION__, L"Failed to get temp direcotry to check file " + filename);

		return false;
	}

	if (filename.find(wcharPath) != std::wstring::npos) {
		return true;
	}

	/* Checking if file is app data file */
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, wcharPath)))
	{
		TempPath = wcharPath;
	}
	else {
		log().error(__FUNCTION__, L"Failed to get app data direcotry to check file " + filename);

		return false;
	}

	return filename.find(wcharPath) != std::wstring::npos;
}
