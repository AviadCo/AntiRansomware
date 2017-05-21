#pragma once
class FunctionCalledHandlerWrapper
{
public:

	virtual void report(int pid, char* functionName, char* param) = 0;

	virtual ~FunctionCalledHandlerWrapper() {};
};

