// ProcessHookMonitorWrapper.h

#pragma once

#include "MessageHandlerWrapper.h"
#include "FunctionCalledHandlerWrapper.h"
#include "ProcessStartEventWrapper.h"
#include "ProcessStopEventWrapper.h"

#ifdef PROCESS_HOOK_MONITOR_WRAPPER
#define PROCESS_HOOK_MONITOR_EXPORT __declspec(dllexport)
#else
#define PROCESS_HOOK_MONITOR_EXPORT __declspec(dllimport)
#endif

namespace ProcessHookMonitorWrapper {

	class PROCESS_HOOK_MONITOR_EXPORT ProcessHookMonitorWrapper
	{
		int pid;
	public: 
		static void initialize();

		static void setStatusHandler(MessageHandlerWrapper* listener);

		static void setFunctionListener(int pid, FunctionCalledHandlerWrapper* listener);

		static void removeFunctionListener(int pid);

		static void inject(int pid, FunctionCalledHandlerWrapper* listener);

		static void close();
		
		/*Process Trace

		static void listenProcessesCreation(ProcessStartEventWrapper* listener);

		static void listenProcessesTermination(ProcessStopEventWrapper * listener);

		static void unlistenProcessesCreation();

		static void unlistenProcessesTermination();*/
	};

	class PROCESS_HOOK_MONITOR_EXPORT ProcessTraceWrapper
	{
	public:

		static void listenProcessesCreation(ProcessStartEventWrapper* listener);

		static void listenProcessesTermination(ProcessStopEventWrapper * listener);

		static void unlistenProcessesCreation();

		static void unlistenProcessesTermination();
	};
}
