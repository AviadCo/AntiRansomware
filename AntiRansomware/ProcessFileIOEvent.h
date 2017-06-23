#pragma once
#include "ProcessEvent.h"
class ProcessFileIOEvent :
	public ProcessEvent
{
public:

	wstring filename;
	wstring fileType;
	bool isFileSame;
	double entropy;
	int entropyGroupSize;

	////////////////////

	bool filenameValid;
	bool fileTypeValid;
	bool isFileSameValid;
	bool entropyValid;
	bool entropyGroupSizeValid;

	ProcessFileIOEvent(ProcessEvent::ProcessOperationTag tag, const wstring& filename, const wstring& fileType, const wstring& isFileSame, const wstring& entropy, const wstring& entropyGroupSize);
	ProcessFileIOEvent(ProcessEvent::ProcessOperationTag tag, const wstring& filename, const wstring& fileType, const wstring& entropy, const wstring& entropyGroupSize);

	~ProcessFileIOEvent();
};

