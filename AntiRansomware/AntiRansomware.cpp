// AntiRansomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <fstream>
#include <iostream>

#include "HoneypotsManager.h"
#include "HoneypotNameGenerator.h"
#include "Honeypot.h"
#include "FileSystemHelper.h"
#include <algorithm>

using std::wstring;
using std::list;
using std::wcout;
using std::cin;
using std::endl;


///////////
#include "FunctionCalledHandlerWrapper.h"
#include "MessageHandlerWrapper.h"
#include "ProcessHookMonitorWrapper.h"
#include "FunctionHooksDefinitions.h"

using namespace FunctionHooksDefinitions;

class cls : public FunctionCalledHandlerWrapper {
	virtual void report(int pid, char* functionName, char* param) {
		std::cout << functionName << endl;
	}
};

class clsMSG : public MessageHandlerWrapper {
	virtual void report(int pid, char* msg) {
		std::cout << msg << endl;
	}
};

int main()
{
	int pid = 572;
	cin >> pid;
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::setStatusHandler(new clsMSG());
	ProcessHookMonitorWrapper::ProcessHookMonitorWrapper::inject(pid, new cls());
	cin >> pid;

	/*
	unsigned int totalTime, intervalTime;
	HoneypotsManager honeypotsManager = HoneypotsManager();

	wcout << L"Creating default number of honeypots" << endl;

	wcout << L"Please Enter total time to run the application in seconds" << endl;
	cin >> totalTime;

	log().info(__FUNCTION__, "Total time from user: " + totalTime);

	wcout << L"Please Enter interval time to run the application in seconds" << endl;
	cin >> intervalTime;

	log().info(__FUNCTION__, "Interval time from user: " + intervalTime);

	if (totalTime < intervalTime) {
		wcout << L"Interval time must be lower than total time, quiting" << endl;
	}

	for (unsigned int i = 0; i < totalTime / intervalTime; ++i) {
		wcout << L"Interval " << i << " started" << endl;

		if (honeypotsManager.monitorHoneypots()) {
			wcout << L"Alert - Honeypot has changed" << endl;
		}
		else {
			wcout << L"Interval finished with no alerts" << endl;
		}

		Sleep(intervalTime * 1000);
	}

	wcout << L"Total time passed, removing honeypots and quiting" << endl;

	honeypotsManager.removeAllHoneypots(); */

	/**
	list<wstring> subDir = FileSystemHelper::getAllFilesInDir(L"C:\\Users\\ransomware\\Downloads");
	wstring firstFile, lastFile;

	FileSystemHelper::getFirstAndLastFileLexicographicOrder(L"C:\\Users\\ransomware\\Downloads", firstFile, lastFile);

	wcout << firstFile << endl;
	wcout << lastFile << endl;

	/*
	std::for_each(subDir.begin(), subDir.end(), [](wstring cur)
	{
		wcout << cur << endl;
	});
*/
	
	Sleep(6000);

    return 0;
}

