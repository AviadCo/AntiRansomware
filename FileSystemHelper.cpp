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
	bool first_sub_dir = true;
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

	hFind = FindFirstFile(directoryPath.c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		log().error(__FUNCTION__, L"Failed to get first file in " + directoryPath + L"Error: " + to_wstring(GetLastError()));

		throw FileSystemHelperException();
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{ 
			if (first_sub_dir) {
				/* jumping on first Dir - it's parent Dir */
				first_sub_dir = false;
				continue;
			}
			wstring subDir = wstring(directoryPath + ffd.cFileName);

			log().debug(__FUNCTION__, L"Found sub directory " + subDir + L" in " + directoryPath);
			
			subDirectories.push_back(subDir);
			subDirectories.merge(getAllSubDirectories(subDir));
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

FileSystemHelper::FileSystemHelper()
{
}


FileSystemHelper::~FileSystemHelper()
{
}
