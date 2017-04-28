#pragma once

#include <string>
#include <list>
#include <exception>

using std::wstring;
using std::list;
using std::exception;

class FileSystemHelper
{
public:
	static list<wstring> getAllSubDirectories(const wstring& directoryPath);
	static list<wstring> getAllFilesInDir(const wstring& directoryPath);
};

class FileSystemHelperException : public exception {};

