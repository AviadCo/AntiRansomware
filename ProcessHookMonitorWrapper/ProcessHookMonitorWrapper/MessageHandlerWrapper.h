#pragma once
class MessageHandlerWrapper
{
public:

	virtual void report(int pid, char* functionName) = 0;
	
	virtual ~MessageHandlerWrapper() {};
};

