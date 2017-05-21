#pragma once
class FunctionCalledHandlerWrapper
{
public:

	virtual void report(int pid, char* functionName) = 0;

	virtual ~FunctionCalledHandlerWrapper() {};
};

