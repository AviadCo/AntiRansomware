#pragma once

#include <windows.h>

#include <string>

using std::wstring;

class Honeypot
{
private:
	wstring lpFileName;

public:
	Honeypot(const wstring lpFileName);
	Honeypot(LPCWSTR lpFileName);
	const wstring getFileName();
	DWORD create();
	bool isChanged();
	bool destroy();
	bool operator==(const Honeypot& other);
	bool operator==(const wstring& other);
	bool operator==(LPCWSTR other);
	
};

