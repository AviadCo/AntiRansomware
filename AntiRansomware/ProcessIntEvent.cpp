#include "stdafx.h"
#include "ProcessIntEvent.h"


ProcessIntEvent::ProcessIntEvent(ProcessEvent::ProcessOperationTag tag, const wstring& pid, const wstring& application, const wstring& commandLine)
	: ProcessEvent(tag) {
	if (!isStringEmpty(application)) {
		this->application = application;
		applicationValid = true;
	}
	else {
		applicationValid = false;
	}

	if (!isStringEmpty(commandLine)) {
		this->commandLine = commandLine;
		commandLineValid = true;
	}
	else {
		commandLineValid = false;
	}

	this->pid = wstringToInt(pid, pidValid);
}

ProcessIntEvent::ProcessIntEvent(ProcessEvent::ProcessOperationTag tag, const wstring& application, const wstring& commandLine)
	: ProcessEvent(tag) {
	if (!isStringEmpty(application)) {
		this->application = application;
		applicationValid = true;
	}
	else {
		applicationValid = false;
	}

	if (!isStringEmpty(commandLine)) {
		this->commandLine = commandLine;
		commandLineValid = true;
	}
	else {
		commandLineValid = false;
	}

	pidValid = false;
}

ProcessIntEvent::ProcessIntEvent(ProcessEvent::ProcessOperationTag tag, const wstring& pid)
	: ProcessEvent(tag) {
		applicationValid = false;
		commandLineValid = false;

	this->pid = wstringToInt(pid, pidValid);
}


ProcessIntEvent::~ProcessIntEvent()
{
}
