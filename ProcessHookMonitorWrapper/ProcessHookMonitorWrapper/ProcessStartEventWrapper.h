#pragma once
#include <Windows.h>

class ProcessStartEventWrapper
{
public:

	virtual void notifyStartEvent(unsigned int pid, unsigned int parentId) = 0;

	virtual ~ProcessStartEventWrapper() {};
};
