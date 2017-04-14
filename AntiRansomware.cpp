// AntiRansomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Logger.h"

#include <string>

#include "HoneypotNameGenerator.h"
#include <list>

using std::string;

int main()
{
	std::list<string> checkList = HoneypotNameGenerator::createFullFileNames();
	log().info("Stating Main!!!");

	for (std::list<string>::iterator it = checkList.begin(); it != checkList.end(); ++it)
		log().info("File name: " + *it);

    return 0;
}

