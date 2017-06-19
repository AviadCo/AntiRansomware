#include "stdafx.h"
#include "ProcessStopEventThunk.h"

ProcessStopEventThunk::ProcessStopEventThunk(ProcessStopEventWrapper * handler)
{
	this->handler = handler;
}

void ProcessStopEventThunk::notify(unsigned int pid)
{
	handler->notifyStopEvent(pid);
}
