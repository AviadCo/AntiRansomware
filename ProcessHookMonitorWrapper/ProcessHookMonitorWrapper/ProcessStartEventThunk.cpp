#include "stdafx.h"
#include "ProcessStartEventThunk.h"

ProcessStartEventThunk::ProcessStartEventThunk(ProcessStartEventWrapper * handler)
{
	this->handler = handler;
}

void ProcessStartEventThunk::notify(unsigned int pid, unsigned int parentId)
{
	handler->notifyStartEvent(pid, parentId);
}
