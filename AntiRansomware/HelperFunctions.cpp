#pragma once
#include "stdafx.h"
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <Windows.h>

#include "HelperFunctions.h"



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

double Antropy::calcAntropy(const std::wstring & filepath)
{
	FILE            *pFile;
	int size = 0;
	int n = 0;
	double entropy = 0.0;
	long            alphabet[256];
	unsigned char   buffer[1024];
	//size_t   buff;

	memset(alphabet, 0, sizeof(long) * 256);

	int result = _wfopen_s(&pFile, filepath.c_str(), L"rb");
	if (result == 0) {
		/* Read the whole file in parts of 1024 */
		while ((n = fread(buffer, 1, 1024, pFile)) != 0)
		{
			/* Add the buffer to the alphabet */
			for (int i = 0; i < n; i++)
			{
				alphabet[(int)buffer[i]]++;
				size++;
			}
		}
		fclose(pFile);

		/* entropy calculation */
		for (int i = 0; i < 256; i++)
		{
			if (alphabet[i] != 0)
			{
				double temp = (float)alphabet[i] / (float)size;
				entropy += -temp * log2(temp);
			}
		}

		return entropy;
	}
	else {
		return -1.0;
	}
}
