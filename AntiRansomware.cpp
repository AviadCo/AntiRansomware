// AntiRansomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Logger.h"
#include "HoneypotNameGenerator.h"

#include <string>
#include <list>

using std::wstring;
using std::list;

int main()
{
	list<wstring> checkList = HoneypotNameGenerator::createFullFileNames();
	log().info(__FUNCTION__, "Stating Main!!!");

	for (list<wstring>::iterator it = checkList.begin(); it != checkList.end(); ++it)
		log().info(__FUNCTION__, L"File name: " + *it);

    return 0;
}

