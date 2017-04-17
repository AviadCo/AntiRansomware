#include "stdafx.h"
#include "HoneypotsManager.h"
#include "HoneypotNameGenerator.h"

void HoneypotsManager::addHoneypots(unsigned int num)
{
	list<wstring> newFilenames = HoneypotNameGenerator::createFullFileNames(num);
	unsigned int i = 0;
	while (i <= num) {
		wstring filename;
		Honeypot tempHP(filename);

		bool tempHPfound = std::find(honeypots.begin(), honeypots.end(), tempHP) != honeypots.end();
		if (!tempHPfound) {
			honeypots.push_front(tempHP);
			tempHP.create();
			registryIO.writeHoneypot(filename);
			i++;
		}
	}
}

void HoneypotsManager::removeHoneypots(unsigned int num)
{
	unsigned int i = 0;
	while (i <= num) {
		Honeypot tempHP = honeypots.front();
		registryIO.removeHoneypot(tempHP.getFileName());
		tempHP.destroy();
		honeypots.pop_front();
		i++;
	}
}

void HoneypotsManager::changeHoneypotsAmountInFileSystem(unsigned int oldSize, unsigned int newSize)
{
	if (newSize > oldSize) {
		addHoneypots(newSize - oldSize);
	}
	else if (newSize < oldSize) {
		removeHoneypots(oldSize - newSize);
	}
}

void HoneypotsManager::loadHoneypotsFromRegistry()
{
	list<wstring> honeypotsFilenames = registryIO.readHoneypots();

	for (wstring const& filename : honeypotsFilenames) {
		Honeypot tempHP(filename);
		honeypots.push_front(tempHP);
	}
	
	honeypotsLimit = honeypots.size();
}

HoneypotsManager::HoneypotsManager()
{
	loadHoneypotsFromRegistry();
	if (honeypots.empty()) {
		this->honeypotsLimit = DEFAULT_HONEYPOTS_LIMIT;
		changeHoneypotsAmountInFileSystem(0, honeypotsLimit);
	}
	
}

HoneypotsManager::HoneypotsManager(unsigned int limit)
{
	loadHoneypotsFromRegistry();
	changeHoneypotsAmountInFileSystem(this->honeypotsLimit, limit);
	this->honeypotsLimit = limit;
}


//todo: here you need to change the amount of honeypots acorrding to the new limit
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

void HoneypotsManager::monitorHoneypots()
{
}

void HoneypotsManager::removeAllHoneypots()
{
	bool errorFlag = false;

	list<Honeypot>::iterator iter = honeypots.begin();
	while (iter != honeypots.end())
	{
		Honeypot& temp = *iter;
		if (temp.destroy()) {
			registryIO.removeHoneypot(temp.getFileName());
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
