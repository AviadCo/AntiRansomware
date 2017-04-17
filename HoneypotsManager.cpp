#include "stdafx.h"

#include <iostream>

#include "HoneypotsManager.h"
#include "HoneypotNameGenerator.h"

using std::wcout;
using std::endl;

void HoneypotsManager::addHoneypot(Honeypot& honeypot)
{
	honeypots.push_front(honeypot);
	honeypot.create();
	registryIO.writeHoneypot(honeypot.getFileName());
}

void HoneypotsManager::removeOneHoneypot()
{
	Honeypot& honeypot = honeypots.front();
	registryIO.removeHoneypot(honeypot.getFileName());
	honeypot.destroy();
	honeypots.pop_front();
}

void HoneypotsManager::addHoneypots(unsigned int num)
{
	list<wstring> newFilenames = HoneypotNameGenerator::createFullFileNames(num);
	unsigned int countAlreadyExist = 0; /* some of the files might exist already */

	for (wstring const& filename : newFilenames) {
		Honeypot tempHP(filename);

		/* check if the given Honeypot already exists */
		bool tempHPfound = std::find(honeypots.begin(), honeypots.end(), tempHP) != honeypots.end();
		if (!tempHPfound) {
			addHoneypot(tempHP);
		}
		else {
			++countAlreadyExist;
		}
	}

	/* some of the file names already exists, generating new names */
	if (countAlreadyExist) {
		addHoneypots(countAlreadyExist);
	}
}

void HoneypotsManager::removeHoneypots(unsigned int num)
{
	unsigned int i = 0;

	while (i <= num) {
		removeOneHoneypot();

		++i;
	}
}

void HoneypotsManager::changeHoneypotsAmountInFileSystem(unsigned int newSize)
{
	unsigned int oldSize = honeypots.size();

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
}

HoneypotsManager::HoneypotsManager()
{
	loadHoneypotsFromRegistry();
	if (honeypots.empty()) {
		changeHoneypotsAmountInFileSystem(DEFAULT_HONEYPOTS_LIMIT);
	}
}

HoneypotsManager::HoneypotsManager(unsigned int numOfHP)
{
	loadHoneypotsFromRegistry();
	changeHoneypotsAmountInFileSystem(numOfHP);
}

bool HoneypotsManager::monitorHoneypots()
{
	bool alert = false;

	for (Honeypot& honeypot : honeypots) {
		if (honeypot.isChanged()) {
			wcout << L"Alert - Honeypot Manager found suspicious act in one of the Honeypots" << endl;
			wcout << L"Honeypot path is: " + honeypot.getFileName() << endl;
			wcout << L"Please be aware that ransom ware might be detected in the system!!!" << endl;

			alert = true;
		}
	}
	
	return alert;
}

void HoneypotsManager::removeAllHoneypots()
{
	changeHoneypotsAmountInFileSystem(0);
}

HoneypotsManager::~HoneypotsManager()
{
}
