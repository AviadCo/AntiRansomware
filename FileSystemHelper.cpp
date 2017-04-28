#include "stdafx.h"

#include "FileSystemHelper.h"
#include "Logger.h"

#include <windows.h>
#include <tchar.h> 
#include <string>
#include <shlwapi.h>

using namespace std;

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