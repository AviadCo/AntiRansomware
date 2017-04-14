#pragma once

#include <windows.h>
#include <string>

using std::string;

class Honeypot
{
private:
	LPCTSTR lpFileName;
	static const string dirUserPictures;
	static const string dirUserDocuments;
	static const string dirUserVideos;
	static const string dirUser;


	void findUserDirectories();

public:
	Honeypot(LPCTSTR lpFileName);
	LPCTSTR getFileName();
	DWORD create();
	bool isChanged();
	bool destroy();
};

