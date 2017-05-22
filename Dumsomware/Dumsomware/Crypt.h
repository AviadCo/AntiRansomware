#pragma once
#include <Windows.h>
#include <string>

class Crypt
{

private:
	static void MyHandleError(LPTSTR psz, int nErrorNumber);


public:
	//-------------------------------------------------------------------
	// Code for the function MyEncryptFile called by main.
	//-------------------------------------------------------------------
	// Parameters passed are:
	//  pszSource, the name of the input, a plaintext file.
	//  pszDestination, the name of the output, an encrypted file to be 
	//   created.
	//  pszPassword, either NULL if a password is not to be used or the 
	//   string that is the password.
	static bool encryptFile(std::wstring SourceFile, std::wstring DestinationFile, std::wstring Password);

};

