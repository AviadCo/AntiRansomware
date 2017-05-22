#include "stdafx.h"
#include "MessageHandlerThunk.h"
#include <Windows.h>


MessageHandlerThunk::MessageHandlerThunk(MessageHandlerWrapper * handler)
{
	this->handler = handler;
}

void MessageHandlerThunk::report(int pid, System::String^ functionName)
{
	handler->report(pid,
		(LPUWSTR)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(functionName).ToPointer());
}
