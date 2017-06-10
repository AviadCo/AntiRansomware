#pragma once

#include <Windows.h>
#include <shlobj.h>
#include <KnownFolders.h>
#include <stdlib.h>

#include <time.h> 

#include <vector>
#include <list>

#include "Logger.h"

using std::list;
using std::vector;

#define MAX_FILENAME_LENGTH 50
#define DEFAULT_NUMBER_OF_HONEYPOTS_NAME 20
#define DEFAULT_NUMBER_OF_DOLFER_IDS 6

static const wstring BITCOIN_WALLET = L"wallet.dat";
static const vector<wstring> FILE_EXTENSTIONS = {
	/* This list of extenstions was taken from
	https://www.cise.ufl.edu/~traynor/papers/scaife-icdcs16.pdf 
	on page 310 */
	L"pdf", L"odt", L"docx", L"pptx", L"txt", L"mov", L"zip",
	L"md", L"nmind", L"opml", L"pages", L"jpg", L"xls", L"csv",
	L"doc", L"ppt", L"gif", L"png", L"xml", L"html", L"xlsx",
	L"mp3", L"jpf", L"log", L"ogg", L"wav",
};

static const vector<int> FILE_EXTENSTIONS_PRIORITY = {
	/* This list of extenstions was taken from
	https://www.cise.ufl.edu/~traynor/papers/scaife-icdcs16.pdf
	on page 310 */
	470, 360, 280, 210, 200, 190, 160,
	150, 150, 150, 150, 90, 80, 30,
	20, 20, 20, 20, 15, 15, 15, 15,
	5, 5, 5, 5,
};

class HoneypotsNameGeneratorFailure : public exception {};

class HoneypotNameGenerator
{
private:
	static int getPriorityByExtenstion(const wstring& ext)
	{
		for (vector<int>::size_type i = 0; i < FILE_EXTENSTIONS.size(); ++i) {
			if (FILE_EXTENSTIONS[i].compare(ext) == 0) {
				return FILE_EXTENSTIONS_PRIORITY[i];
			}
		}

		return 0;
	}
public:
	static int getFilePriority(const wstring& fileName)
	{
		std::wstring::size_type extIndex = fileName.find_first_of(L".");

		return getPriorityByExtenstion(fileName.substr(extIndex + 1));
	}

	static wstring HoneypotNameGenerator::getRandomFileExtenstion()
	{
		srand(time(NULL));

		return FILE_EXTENSTIONS[rand() % (FILE_EXTENSTIONS.size() - 1)];
	}

	static wstring HoneypotNameGenerator::getFileExtenstion(unsigned int extension)
	{
		if (extension >= FILE_EXTENSTIONS.size()) {
			log().error(__FUNCTION__, L"Failed to get file extension with index " + extension);

			throw HoneypotsNameGeneratorFailure();
		}

		return FILE_EXTENSTIONS[extension];
	}

	static wstring HoneypotNameGenerator::generateRandomFileName(unsigned int len, wstring fileExtenstion) {
		wstring fileName;

		if (len > MAX_FILENAME_LENGTH) {
			log().error(__FUNCTION__, L"Failed to generate random file name, file name length is bigger than " + MAX_FILENAME_LENGTH);

			throw HoneypotsNameGeneratorFailure();
		}

		static const wchar_t alphanum[] =
			L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

		srand(time(NULL));
		for (unsigned int i = 0; i < len; ++i) {
			fileName += alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		return fileName + L"." + fileExtenstion;
	}

	static wstring HoneypotNameGenerator::generateRandomFileName(wstring fileExtenstion) {
		srand(time(NULL));

		return generateRandomFileName(rand() % MAX_FILENAME_LENGTH, fileExtenstion);
	}

	static wstring HoneypotNameGenerator::getDirectoryPath(REFKNOWNFOLDERID rdid)
	{
		PWSTR path = NULL;
		wstring pathStr = L"";

		if (SUCCEEDED(SHGetKnownFolderPath(rdid, 0, NULL, &path))) {
			pathStr += path;

			log().debug(__FUNCTION__, L"Succeeded to get folder path: " + pathStr);

			/* must call free on SHGetKnownFolderPath success */
			CoTaskMemFree(path);
		}
		else {
			log().error(__FUNCTION__, L"Failed to get folder path to create Honeypot name.");

			throw HoneypotsNameGeneratorFailure();
		}

		return pathStr + L"\\";
	}

	static wstring HoneypotNameGenerator::getFullFileName(wstring fileName, REFKNOWNFOLDERID rdid)
	{
		return getDirectoryPath(rdid) + fileName;
	}

	static wstring HoneypotNameGenerator::getRandomFullFileName(wstring fileExtenstion, REFKNOWNFOLDERID rdid)
	{
		return getDirectoryPath(rdid) + generateRandomFileName(fileExtenstion);
	}

	static wstring HoneypotNameGenerator::getRandomFullFileName(REFKNOWNFOLDERID rdid)
	{
		return getRandomFullFileName(getRandomFileExtenstion(), rdid);
	}

	static REFKNOWNFOLDERID getRandomFolderID()
	{
		srand(time(NULL));

		switch ((rand() % DEFAULT_NUMBER_OF_DOLFER_IDS)) {
		case 0:
			return FOLDERID_Desktop;

		case 1:
			return FOLDERID_Documents;

		case 2:
			return FOLDERID_Downloads;

		case 3:
			return FOLDERID_Favorites;

		case 4:
			return FOLDERID_Music;

		case 5:
			return FOLDERID_Pictures;
		}
	}

	static list<wstring> HoneypotNameGenerator::createFullFileNames(unsigned int num_of_files)
	{
		list<wstring> filesList = list<wstring>();

		for (unsigned int i = 0; i < num_of_files; ++i) {
			wstring newFileName = getRandomFullFileName(getRandomFolderID());

			log().info(__FUNCTION__, L"New Honeypot name created: " + newFileName);

			filesList.push_back(newFileName);
		}

		return filesList;
	}

	static list<wstring> HoneypotNameGenerator::createFullFileNames()
	{
		return createFullFileNames(DEFAULT_NUMBER_OF_HONEYPOTS_NAME);
	}

};