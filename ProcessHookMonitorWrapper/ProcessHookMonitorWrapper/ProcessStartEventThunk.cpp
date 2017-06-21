#include "stdafx.h"
#include "ProcessStartEventThunk.h"

ProcessStartEventThunk::ProcessStartEventThunk(ProcessStartEventWrapper * handler)
{
	this->handler = handler;
}

void ProcessStartEventThunk::notify(unsigned int pid, System::String^ functionName, unsigned int parentId)
{
	handler->notifyStartEvent(pid,
		(LPUWSTR)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(functionName).ToPointer(),
		parentId);
}
