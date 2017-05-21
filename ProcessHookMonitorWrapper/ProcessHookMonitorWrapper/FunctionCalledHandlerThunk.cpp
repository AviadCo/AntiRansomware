#include "stdafx.h"
#include "FunctionCalledHandlerThunk.h"

FunctionCalledHandlerThunk::FunctionCalledHandlerThunk(FunctionCalledHandlerWrapper * handler)
{
	this->handler = handler;
}

void FunctionCalledHandlerThunk::report(int pid, System::String^ functionName)
{
	handler->report(pid,
		(char *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(functionName).ToPointer());
}
