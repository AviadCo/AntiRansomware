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

class func : public FunctionCalledHandlerWrapper {
	virtual void report(int pid, LPUWSTR functionName, LPUWSTR param) {
		std::wstring name(functionName);
		std::wstring par(param);
		std::wcout << functionName /*<< " , " << param */ << std::endl << std::flush;
	}
};

int main()
{
	
	char c;
	int pid;
	std::cin >> pid;
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::initialize();
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject(pid, new func());

	std::cout << "hi";
	std::cin >> c;
    return 0;
}

