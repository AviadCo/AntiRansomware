#pragma once

#include <list>
#include <exception>

#include "Honeypot.h"
#include "RegistryIO.h"

using std::list;
using std::exception;

#define DEFAULT_HONEYPOTS_LIMIT 10

class HoneypotsManager
{
private:
	RegistryIO registryIO;
	list<Honeypot> honeypots;
	unsigned int honeypotsLimit;
	
	void addHoneypots(unsigned int num);
	void removeHoneypots(unsigned int num);
	void changeHoneypotsAmountInFileSystem(unsigned int oldSize, unsigned int newSize);
	void loadHoneypotsFromRegistry();

public:
	HoneypotsManager();
	HoneypotsManager(unsigned int limit);
	void setLimitToHoneypots(unsigned int newLimit);
	void monitorHoneypots();
	void removeAllHoneypots();
	~HoneypotsManager();
};

class HoneypotsManagerExceptoins : public exception{};
class CantRemoveHoneypot : public HoneypotsManagerExceptoins {};

