#pragma once

#include <windows.h>

#include <exception>

using std::exception;

/* This represent an analyzer for one process in the system */
class ProcessAnalyzer
{
private:
	HANDLE processHandle;
	HANDLE parentHandle;

public:
	ProcessAnalyzer(DWORD proccessID);
	~ProcessAnalyzer();
	DWORD getProcessID();
	DWORD getParentID();
};

class ProcessAnalyzerException : public exception {};

