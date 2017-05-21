// This is the main DLL file.

#include "stdafx.h"

#include "ProcessHookMonitorWrapper.h"
#include "MessageHandlerThunk.h"
#include "FunctionCalledHandlerThunk.h"

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(MessageHandlerWrapper* listener)
{

	MessageHandlerThunk ^thunk = gcnew MessageHandlerThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessHookMonitor::setStatusHandler(
		gcnew ProcessHookMonitor::MessageHandler(thunk, &MessageHandlerThunk::report));
	 
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setFunctionListener(int pid, FunctionCalledHandlerWrapper * listener)
{
	FunctionCalledHandlerThunk ^thunk = gcnew FunctionCalledHandlerThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessHookMonitor::setFunctionListener(
		pid, gcnew ProcessHookMonitor::FunctionCalledHandler(thunk, &FunctionCalledHandlerThunk::report));
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::removeFunctionListener(int pid)
{
	ProcessHookMonitor::ProcessHookMonitor::removeFunctionListener(pid);
}

void ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject(int pid, FunctionCalledHandlerWrapper * listener)
{
	FunctionCalledHandlerThunk ^thunk = gcnew FunctionCalledHandlerThunk(listener);

	// Bind the ontick event with the thunk
	ProcessHookMonitor::ProcessHookMonitor::inject(
		pid, gcnew ProcessHookMonitor::FunctionCalledHandler(thunk, &FunctionCalledHandlerThunk::report));
}
