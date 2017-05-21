#include "stdafx.h"
#include "MessageHandlerThunk.h"




MessageHandlerThunk::MessageHandlerThunk(MessageHandlerWrapper * handler)
{
	this->handler = handler;
}

void MessageHandlerThunk::report(int pid, System::String^ functionName)
{
	handler->report(pid,
		(char *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(functionName).ToPointer());
}
