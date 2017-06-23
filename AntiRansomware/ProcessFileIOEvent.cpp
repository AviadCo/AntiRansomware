#include "stdafx.h"
#include "ProcessFileIOEvent.h"

	ProcessFileIOEvent::ProcessFileIOEvent(ProcessEvent::ProcessOperationTag tag, const wstring& filename, const wstring& fileType, const wstring& isFileSame, const wstring& entropy, const wstring& entropyGroupSize)
		: ProcessEvent(tag) {

		if (!isStringEmpty(filename)) {
			this->filename = filename;
			filenameValid = true;
		}
		else {
			filenameValid = false;
		}

		if (!isStringEmpty(fileType)) {
			this->fileType = fileType;
			fileTypeValid = true;
		}
		else {
			fileTypeValid = false;
		}

		this->isFileSame = wstringToBool(isFileSame, isFileSameValid);
		this->entropy = wstringToDouble(entropy, entropyValid);
		this->entropyGroupSize = wstringToInt(entropyGroupSize, entropyGroupSizeValid);

	}

	ProcessFileIOEvent::ProcessFileIOEvent(ProcessEvent::ProcessOperationTag tag, const wstring& filename, const wstring& fileType, const wstring& entropy, const wstring& entropyGroupSize)
		: ProcessEvent(tag) {
		if (!isStringEmpty(filename)) {
			this->filename = filename;
			filenameValid = true;
		}
		else {
			filenameValid = false;
		}

		if (!isStringEmpty(fileType)) {
			this->fileType = fileType;
			fileTypeValid = true;
		}
		else {
			fileTypeValid = false;
		}

		this->entropy = wstringToDouble(entropy, entropyValid);
		this->entropyGroupSize = wstringToInt(entropyGroupSize, entropyGroupSizeValid);
		this->isFileSameValid = false;
	}


ProcessFileIOEvent::~ProcessFileIOEvent()
{
}
