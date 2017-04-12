#include "stdafx.h"
#include "HoneypotsManager.h"

#define DEFAULT_HONEYPOTS_LIMIT 10

HoneypotsManager::HoneypotsManager()
{
	this->honeypotsLimit = DEFAULT_HONEYPOTS_LIMIT;
}

HoneypotsManager::HoneypotsManager(unsigned int limit)
{
	this->honeypotsLimit = limit;
}

void HoneypotsManager::setLimitToHoneypots(unsigned int newLimit)
{
	if (newLimit > honeypotsLimit) {
		//delutes the honeypots to fit the new limit
		list<Honeypot>::iterator iter = honeypots.begin();
		while (honeypots.size() > newLimit)
		{
			Honeypot& temp = *iter;

			if (temp.destroy()) {
				iter = honeypots.erase(iter);
			}
			else {
				iter++;
			}
		}
	}

	honeypotsLimit = newLimit;
}

DWORD HoneypotsManager::deployHoneypots()
{
	return 0;
}

void HoneypotsManager::monitorHoneypots()
{
}

void HoneypotsManager::removeHoneypots()
{
	bool errorFlag = false;

	list<Honeypot>::iterator iter = honeypots.begin();
	while (iter != honeypots.end())
	{
		Honeypot& temp = *iter;
		if (temp.destroy()) {
			iter = honeypots.erase(iter);
		} 
		else {
			errorFlag = true;
			iter++;
		}
	}

	if (errorFlag) {
		throw CantRemoveHoneypot();
	}
}

HoneypotsManager::~HoneypotsManager()
{
}
