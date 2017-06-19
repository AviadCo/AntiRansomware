#pragma once
#include "ProcessStopEventWrapper.h"

ref class ProcessStopEventThunk
{
	ProcessStopEventWrapper* handler;
public:
	ProcessStopEventThunk(ProcessStopEventWrapper* handler);

	void notify(unsigned int pid);
};