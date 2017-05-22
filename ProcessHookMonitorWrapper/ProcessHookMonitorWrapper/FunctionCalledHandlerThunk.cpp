#include "stdafx.h"
#include "FunctionCalledHandlerThunk.h"
#include <Windows.h>

FunctionCalledHandlerThunk::FunctionCalledHandlerThunk(FunctionCalledHandlerWrapper * handler)
{
	this->handler = handler;
}

void FunctionCalledHandlerThunk::report(int pid, System::String^ functionName, System::String^ param)
{
	
	handler->report(pid,
		(LPUWSTR)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(functionName).ToPointer(),
		(LPUWSTR)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(param).ToPointer());
}



