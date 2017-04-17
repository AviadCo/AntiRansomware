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
	//************************************
	// Method:    getFileName
	// FullName:  Honeypot::getFileName
	// Access:    public 
	// Returns:   const wstring - the path of the Honeypot
	// Qualifier:
	//************************************
	const wstring getFileName();
	//************************************
	// Method:    create - create new Honeypot file in the system
	// FullName:  Honeypot::create
	// Access:    public 
	// Returns:   DWORD - 0 for success, otherwise errno number
	// Qualifier:
	//************************************
	DWORD create();
	//************************************
	// Method:    isChanged - true if the Honeypot changed, otherwise false
	// FullName:  Honeypot::isChanged
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	//************************************
	bool isChanged();
	//************************************
	// Method:    destroy - delete the Honeypot from system
	// FullName:  Honeypot::destroy
	// Access:    public 
	// Returns:   bool - true if deletion succeeded, otherwise  false 
	// Qualifier:
	//************************************
	bool destroy();
	bool operator==(const Honeypot& other);
	bool operator==(const wstring& other);
	bool operator==(LPCWSTR other);
	
};

