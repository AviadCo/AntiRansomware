#pragma once
#include <Windows.h>
#include <list>
#include <exception>

#include "Honeypot.h"

using std::list;
using std::exception;

class HoneypotsManager
{
private:
	list<Honeypot> honeypots;
	unsigned int honeypotsLimit;


public:
	HoneypotsManager();
	HoneypotsManager(unsigned int limit);
	void setLimitToHoneypots(unsigned int newLimit);
	DWORD deployHoneypots();
	void monitorHoneypots();
	void removeHoneypots();
	~HoneypotsManager();
};

class HoneypotsManagerExceptoins : public exception{};
class CantRemoveHoneypot : public HoneypotsManagerExceptoins {};

