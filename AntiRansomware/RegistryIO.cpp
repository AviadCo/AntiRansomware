#include "stdafx.h"
#include "RegistryIO.h"
#include "Logger.h"

using std::to_string;

HKEY RegistryIO::openKey(HKEY where, wstring name) {
	HKEY resKey;
	LONG nError = RegOpenKey(where, name.c_str(), &resKey);

	if (nError == ERROR_NO_MATCH || nError == ERROR_FILE_NOT_FOUND) {
		nError = RegCreateKeyW(where, name.c_str(), &resKey);
		
		if (nError != ERROR_SUCCESS) {
			log().error(__FUNCTION__, L"can't create key " + name);
			throw CantOpenKey();
		}

		log().info(__FUNCTION__, L"key created: " + name);
		return resKey;

	} else if (ERROR_SUCCESS != nError) {
		log().error(__FUNCTION__, L"can't open key " + name);
		throw CantOpenKey();
	}

	log().info(__FUNCTION__, L"key opened: " + name);

	return resKey;
}

void RegistryIO::deleteKey(HKEY where, wstring name)
{
	LONG nError = RegDeleteKey(where, name.c_str());

	if (nError != ERROR_SUCCESS) {
		log().error(__FUNCTION__, L"can't delete key " + name);
		throw CantDeleteKey();
	}

	log().info(__FUNCTION__, L"delete Key " + name + L" succesfully");
}

void RegistryIO::closeKey(HKEY where)
{
	RegCloseKey(where);
}

void RegistryIO::setValueData(HKEY where, wstring name, wstring value)
{
	DWORD cbData = (value.length() + 1) * sizeof(WCHAR);
	LONG nError = RegSetKeyValue(where, NULL, name.c_str(), REG_SZ, value.c_str(), cbData);

	if (nError != ERROR_SUCCESS) {
		log().error(__FUNCTION__, L"can't set value " + name);
		throw CantSetValue();
	}

	log().info(__FUNCTION__, L"set value " + name + L" succesfully");
}

wstring RegistryIO::getValueData(HKEY where, wstring name)
{
	LONG size;
	LPWSTR buffer;
		
	//get the size of the data to read
	LONG nError = RegQueryValue(where, name.c_str(), NULL, &size);

	if (nError != ERROR_SUCCESS) {
		log().error(__FUNCTION__, L"Could not get registry value: " + name);
		throw CantGetValue();
	}

	buffer = new WCHAR[size];
		
	//read the data
	nError = RegQueryValue(where, name.c_str(), buffer, &size);

	if (nError != ERROR_SUCCESS) {
		log().error(__FUNCTION__, L"Could not get registry value: " + name);
		delete[] buffer;
		throw CantGetValue();
	}

	wstring res(buffer);
	delete[] buffer;

	return res;
}

list<wstring> RegistryIO::getAllValuesNames(HKEY where)
{
	list<wstring> result;
	DWORD numOfValues;

	//get the number of values in the given key
	LONG nError = RegQueryInfoKey(where, NULL, NULL, NULL, NULL, NULL, NULL, &numOfValues,
		NULL, NULL, NULL, NULL);

	if (nError != ERROR_SUCCESS) {
		log().error(__FUNCTION__, L"Could not get values from given key");
		throw CantGetValue();
	}

	log().debug(__FUNCTION__, L"number of values: " + std::to_wstring(numOfValues));

	//retrieve value's names under given key
	if (numOfValues > 0) {	
		for (unsigned int i = 0; i < numOfValues; i++) {
			WCHAR nameBuff[MAX_VALUE_NAME];
			DWORD sizeBuff = MAX_VALUE_NAME;
			nameBuff[0] = L'\0';

			nError = RegEnumValue(where, i, nameBuff, &sizeBuff, NULL, NULL, NULL, NULL);

			if (nError == ERROR_SUCCESS){
				wstring temp(nameBuff);
				result.push_back(temp);
				log().debug(__FUNCTION__, L"get value name: " + temp);
			}
			else {
				log().error(__FUNCTION__, "error while try to get value number " + to_string(i));
			}
		}
	}

	return result;
}

void RegistryIO::deleteValueByName(HKEY where, wstring name)
{
	LONG nError = RegDeleteValue(where, name.c_str());

	if (nError != ERROR_SUCCESS) {
		log().error(__FUNCTION__, L"can't delete value: " + name);
		
		throw CantDeleteValue();
	}

	log().debug(__FUNCTION__, L"value: " + name + L" deleted successfully");
}


RegistryIO::RegistryIO() : regkeyHome(openKey(HKEY_CURRENT_USER, REGISTRY_KEY_HOME)), 
	regkeyHonypots(openKey(regkeyHome, REGISTRY_KEY_HONEYPOTS))
{ 
}

void RegistryIO::writeHoneypot(Honeypot honeypot)
{
	setValueData(regkeyHonypots, honeypot.getFileName(), L"");
}

list<wstring> RegistryIO::readHoneypots()
{
	return getAllValuesNames(regkeyHonypots);
}

void RegistryIO::removeHoneypot(Honeypot honeypot)
{
	deleteValueByName(regkeyHonypots, honeypot.getFileName());
}


RegistryIO::~RegistryIO()
{
	closeKey(regkeyHonypots);
	closeKey(regkeyHome);
}
