#pragma once
#include "stdafx.h"

#include "StringFunctions.h"



std::vector<std::wstring> StringFunctions::split(const std::wstring &sourceStr,
	const std::wstring &delimiter,
	bool ignoreEmpty)
{
	std::vector<std::wstring> resultVector;

	size_t idxA = 0;
	size_t idxB = sourceStr.find(delimiter);
	std::wstring tempStr;
	bool done = false;
	
	while (!done)
	{
		if (idxB != std::wstring::npos)
		{
			tempStr = sourceStr.substr(idxA, idxB - idxA);
			idxA = idxB + delimiter.length();
			idxB = sourceStr.find(delimiter, idxA);
		}
		else
		{
			tempStr = sourceStr.substr(idxA);
			done = true;
		}

		if (!(ignoreEmpty && tempStr.empty()))
			resultVector.push_back(tempStr);
	}

	return resultVector;
}

std::vector<std::wstring> StringFunctions::splitParam(const std::wstring &sourceStr)
{
	return split(sourceStr, std::wstring(L"|/|/|"), true);
}
