#pragma once

#include "FunctionCalledHandlerWrapper.h"

ref class FunctionCalledHandlerThunk
{
	FunctionCalledHandlerWrapper* handler;
public:
	FunctionCalledHandlerThunk(FunctionCalledHandlerWrapper* handler);

	void report(int pid, System::String^ functionName);
};

