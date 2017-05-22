#pragma once
#include <Windows.h>
class FunctionCalledHandlerWrapper
{
public:

	virtual void report(int pid, LPUWSTR functionName, LPUWSTR param) = 0;

	virtual ~FunctionCalledHandlerWrapper() {};
};

