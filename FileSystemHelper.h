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
	FileSystemHelper();
	~FileSystemHelper();
};

class FileSystemHelperException : public exception {};

