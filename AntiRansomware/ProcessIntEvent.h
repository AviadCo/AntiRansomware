#pragma once
#include "ProcessEvent.h"
#include <string>

using std::wstring;

class ProcessIntEvent :
	public ProcessEvent
{
	int pid;
	wstring application;
	wstring commandLine;

	//////////////////////////////////////////

	bool pidValid;
	bool applicationValid;
	bool commandLineValid;

public:
	ProcessIntEvent(ProcessEvent::ProcessOperationTag tag, const wstring& pid, const wstring& application, const wstring& commandLine);
	ProcessIntEvent(ProcessEvent::ProcessOperationTag tag, const wstring& application, const wstring& commandLine);
	ProcessIntEvent(ProcessEvent::ProcessOperationTag tag, const wstring& pid);
	
	~ProcessIntEvent();
};

