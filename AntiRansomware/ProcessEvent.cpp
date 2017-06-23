#include "stdafx.h"
#include "ProcessEvent.h"


ProcessEvent::ProcessEvent(ProcessOperationTag tag) {
	this->tag = tag;
}

bool ProcessEvent::isStringEmpty(const wstring& str) {
	return str.length() == 0;
}

double ProcessEvent::wstringToDouble(const wstring& str, bool& validFlag) {
	validFlag = true;

	if (isStringEmpty(str))
	{
		validFlag = false;
		return -1;
	}

	double result = _wtof(str.c_str());

	if (result < 0) {
		validFlag = false;
	}

	return result;
}

int ProcessEvent::wstringToInt(const wstring& str, bool& validFlag) {
	validFlag = true;

	if (str.compare(L"") == 0)
	{
		validFlag = false;
		return -1;
	}
	
	int result = _wtoi(str.c_str());

	if (result < 0) {
		validFlag = false;
	}

	return result;
}

bool ProcessEvent::wstringToBool(const wstring& str, bool& validFlag) {
	validFlag = true;

	if (str.compare(L"") == 0)
	{
		validFlag = false;
		return -1;
	}

	int result = _wtoi(str.c_str());

	if (result < 0) {
		validFlag = false;
	}

	return result == 1;
}