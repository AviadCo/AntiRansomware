#include "stdafx.h"
#include "ProcessFileOpEvent.h"

ProcessFileOpEvent::ProcessFileOpEvent(ProcessEvent::ProcessOperationTag tag, const wstring& src, const wstring& target)
	: ProcessEvent(tag) {

	if (!isStringEmpty(src)) {
		this->src = src;
		srcValid = true;
	}
	else {
		srcValid = false;
	}

	if (!isStringEmpty(target)) {
		this->target = target;
		targetValid = true;
	}
	else {
		targetValid = false;
	}
	

}

ProcessFileOpEvent::ProcessFileOpEvent(ProcessEvent::ProcessOperationTag tag, const wstring& target)
	: ProcessEvent(tag) {

		srcValid = false;

	if (!isStringEmpty(target)) {
		this->target = target;
		targetValid = true;
	}
	else {
		targetValid = false;
	}


}

ProcessFileOpEvent::~ProcessFileOpEvent()
{
}
