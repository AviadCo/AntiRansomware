#pragma once

#include <Windows.h>

#include <string>
#include <list>
#include <exception>

using std::wstring;
using std::list;
using std::exception;

#define REGISTRY_KEY_HOME L"software\\AntiRansom"
#define REGISTRY_KEY_HONEYPOTS L"HoPs"
#define MAX_VALUE_NAME 16383

class RegistryIO
{
private:
	const HKEY regkeyHome;
	const HKEY regkeyHonypots;

	/*
	* wrapper functions to WINAPI
	*/
	static HKEY openKey(HKEY where, wstring name);
	static void deleteKey(HKEY where, wstring name);
	static void closeKey(HKEY where);
	static void setValueData(HKEY where, wstring name, wstring value);
	static wstring getValueData(HKEY where, wstring name);
	static list<wstring> getAllValuesNames(HKEY where);
	static void deleteValueByName(HKEY where, wstring name);

	/*
	* private functions
	*/


	/*
	* public functions
	*/
public:
	RegistryIO();
	//************************************
	// Method:    writeHoneypot
	// FullName:  RegistryIO::writeHoneypot
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: wstring path - save the Honeypot path in the registry.
	//************************************
	void writeHoneypot(wstring path);
	//************************************
	// Method:    readHoneypots
	// FullName:  RegistryIO::readHoneypots
	// Access:    public 
	// Returns:   list<wstring> - list of all stored Honeypots in the system
	// Qualifier:
	//************************************
	list<wstring> readHoneypots();
	//************************************
	// Method:    removeHoneypot
	// FullName:  RegistryIO::removeHoneypot
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: wstring path - the path of the Honeypot to be removed
	//************************************
	void removeHoneypot(wstring path);
	~RegistryIO();
};

/*
	Exceptions:
*/

class RegistryIOExceptions : public exception {};
class CantOpenKey : public RegistryIOExceptions {};
class CantDeleteKey : public RegistryIOExceptions {};
class CantSetValue : public RegistryIOExceptions {};
class CantGetValue : public RegistryIOExceptions {};
class CantDeleteValue : public RegistryIOExceptions {};

