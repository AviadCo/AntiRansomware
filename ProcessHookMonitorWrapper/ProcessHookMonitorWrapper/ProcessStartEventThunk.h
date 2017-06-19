#pragma once
#include "ProcessStartEventWrapper.h"

ref class ProcessStartEventThunk
{
	ProcessStartEventWrapper* handler;
public:
	ProcessStartEventThunk(ProcessStartEventWrapper* handler);

	void notify(unsigned int pid, unsigned int parentId);
};
