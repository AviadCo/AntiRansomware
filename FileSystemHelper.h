#pragma once

#include <string>
#include <list>
#include <exception>
#include <functional>

#include <windows.h>
#include <tchar.h> 
#include <string>
#include <shlwapi.h>

using std::wstring;
using std::list;
using std::exception;

class FileSystemHelper
{
private:
	static void getFirstAndLastByCoparable(const wstring & directoryPath, wstring & firstFile, wstring & lastFile, std::function<bool(const WIN32_FIND_DATA &, const WIN32_FIND_DATA &)> compare);
public:
	static list<wstring> getAllSubDirectories(const wstring& directoryPath);
	static list<wstring> getAllFilesInDir(const wstring& directoryPath);
	static void getFirstAndLastFileLexicographicOrder(const wstring& directoryPath, wstring& firstFile, wstring& lastFile);
	static void getFirstAndLastAccessTimeOrder(const wstring& directoryPath, wstring& firstFile, wstring& lastFile);
	static void getFirstAndLastFileWriteTimeOrder(const wstring& directoryPath, wstring& firstFile, wstring& lastFile);
};

class FileSystemHelperException : public exception {};

