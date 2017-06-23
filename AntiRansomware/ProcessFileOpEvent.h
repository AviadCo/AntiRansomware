#pragma once
#include "ProcessEvent.h"
#include <string>

using std::wstring;

class ProcessFileOpEvent :
	public ProcessEvent
{
public:
	wstring src;
	wstring target;

	//valid flags
	bool srcValid;
	bool targetValid;

	ProcessFileOpEvent(ProcessEvent::ProcessOperationTag tag, const wstring& src, const wstring& target);
	ProcessFileOpEvent(ProcessEvent::ProcessOperationTag tag, const wstring& target);
	~ProcessFileOpEvent();
};

