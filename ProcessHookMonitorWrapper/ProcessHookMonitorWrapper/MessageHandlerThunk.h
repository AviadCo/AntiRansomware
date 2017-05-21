#pragma once
#include "MessageHandlerWrapper.h"

ref class MessageHandlerThunk
{
	MessageHandlerWrapper* handler;
public:
	MessageHandlerThunk(MessageHandlerWrapper* handler);

	void report(int pid, System::String^ functionName);
};

