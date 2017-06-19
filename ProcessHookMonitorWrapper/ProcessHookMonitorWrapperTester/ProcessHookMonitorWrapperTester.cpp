// ProcessHookMonitorWrapperTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#include <string>
#include "FunctionCalledHandlerWrapper.h"
#include "MessageHandlerWrapper.h"
#include "ProcessHookMonitorWrapper.h"
#include "ProcessStartEventWrapper.h"
#include "ProcessStopEventWrapper.h"

class func : public FunctionCalledHandlerWrapper {
	virtual void report(int pid, LPUWSTR functionName, LPUWSTR param) {
		std::wstring name(functionName);
		std::wstring par(param);
		std::wcout << functionName /*<< " , " << param */ << std::endl << std::flush;
	}
};

class startHandle : public ProcessStartEventWrapper {
	virtual void notifyStartEvent(unsigned int pid, unsigned int parentId) {
		std::wcout << pid << " , " << parentId  << std::endl << std::flush;
	}
};

class stopHandle : public ProcessStopEventWrapper {
	virtual void notifyStopEvent(unsigned int pid) {
		std::wcout << pid << std::endl << std::flush;
	}
};

int main()
{
	
	char c;
	int pid;
	std::cin >> pid;

	//ProcessHookMonitorWrapper::ProcessTraceWrapper::listenProcessesCreation(new startHandle());
	//ProcessHookMonitorWrapper::ProcessTraceWrapper::listenProcessesTermination( new stopHandle());
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::initialize();
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject(pid, new func());

	std::cout << "hi";
	std::cin >> c;
	//ProcessHookMonitorWrapper::ProcessTraceWrapper::unlistenProcessesCreation();
	//ProcessHookMonitorWrapper::ProcessTraceWrapper::unlistenProcessesTermination();
    return 0;
}

