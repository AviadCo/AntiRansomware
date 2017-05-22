#pragma once
#include <Windows.h>

class MessageHandlerWrapper
{
public:

	virtual void report(int pid, LPUWSTR functionName) = 0;
	
	virtual ~MessageHandlerWrapper() {};
};

