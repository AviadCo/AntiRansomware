#include "stdafx.h"
#include "FunctionCalledHandlerThunk.h"

FunctionCalledHandlerThunk::FunctionCalledHandlerThunk(FunctionCalledHandlerWrapper * handler)
{
	this->handler = handler;
}

void FunctionCalledHandlerThunk::report(int pid, System::String^ functionName, System::String^ param)
{
	handler->report(pid,
		(char *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(functionName).ToPointer(),
		(char *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(param).ToPointer());
}
