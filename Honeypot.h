#pragma once
class Honeypot
{
private:
	LPCTSTR lpFileName;

public:
		Honeypot(LPCTSTR lpFileName);
		LPCTSTR getFileName();
		DWORD create();
		bool isChanged();
		bool destory();
};

