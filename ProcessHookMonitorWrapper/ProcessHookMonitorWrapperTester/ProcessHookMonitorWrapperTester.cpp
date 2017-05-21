// ProcessHookMonitorWrapperTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "FunctionCalledHandlerWrapper.h"
#include "MessageHandlerWrapper.h"
#include "ProcessHookMonitorWrapper.h"

class func : public FunctionCalledHandlerWrapper {
	virtual void report(int pid, char* functionName, char* param) {
		std::cout << functionName << " , " << param << std::endl;
	}
};

int main()
{
	char c;
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject(12860, new func());

	std::cout << "hi";
	std::cin >> c;
    return 0;
}

