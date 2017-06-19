#pragma once
#include <Windows.h>

class ProcessStopEventWrapper
{
public:

	virtual void notifyStopEvent(unsigned int pid) = 0;

	virtual ~ProcessStopEventWrapper() {};
};