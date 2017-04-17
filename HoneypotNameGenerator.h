#pragma once

#include <Windows.h>
#include <shlobj.h>
#include <KnownFolders.h>

#include <vector>
#include <list>

#include "Logger.h"

using std::list;
using std::vector;

#define MAX_FILENAME_LENGTH 50
#define DEFAULT_NUMBER_OF_HONEYPOTS_NAME 20
#define DEFAULT_NUMBER_OF_DOLFER_IDS 7

static const wstring BITCOIN_WALLET = L"wallet.dat";
static const vector<wstring> FILE_EXTENSTIONS = {
	L"mid", L"wma", L"flv", L"mkv", L"mov", L"avi", L"asf", L"mpeg", L"vob",
	L"mpg", L"wmv", L"fla", L"swf", L"wav", L"qcow2", L"vdi", L"vmdk",
	L"vmx", L"gpg", L"aes", L"ARC", L"PAQ", L"tar.bz2", L"tbk", L"bak", L"tar",
	L"tgz", L"rar", L"zip", L"djv", L"djvu", L"svg", L"bmp", L"png", L"gif", L"raw",
	L"cgm", L"jpeg", L"jpg", L"tif", L"tiff", L"NEF", L"psd", L"cmd", L"bat", L"class",
	L"jar", L"java", L"asp", L"brd", L"sch", L"dch", L"dip", L"vbs", L"asm", L"pas", L"cpp",
	L"php", L"ldf", L"mdf", L"ibd", L"MYI", L"MYD", L"frm", L"odb", L"dbf", L"mdb",
	L"sql", L"SQLITEDB", L"SQLITE3", L"asc", L"lay6", L"lay", L"ms11", L"sldm",
	L"sldx", L"ppsm", L"ppsx", L"ppam", L"pptm", L"std", L"sxd", L"pot", L"pps",
	L"sti", L"sxi", L"otp", L"odp", L"wks", L"xltx", L"xltm", L"xlsx", L"xlsm",
	L"slk", L"xlw", L"xlt", L"xlm", L"xlc", L"dif", L"stc", L"sxc", L"ots",
	L"vods", L"hwp", L"dotm", L"dotx", L"docm", L"docx", L"DOT", L"max", L"xml",
	L"txt", L"CSV", L"uot", L"RTF", L"pdf", L"XLS", L"PPT", L"stw", L"sxw",
	L"ott", L"odt", L"DOC" , L"pem", L"csr", L"crt", L"kev",
};

class HoneypotsNameGeneratorFailure : public exception {};

class HoneypotNameGenerator
{
private:

public:
	HoneypotNameGenerator();
	~HoneypotNameGenerator();
	static wstring HoneypotNameGenerator::getRandomFileExtenstion()
	{
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
			L"0123456789"
			L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			L"abcdefghijklmnopqrstuvwxyz";

		for (unsigned int i = 0; i < len; ++i) {
			fileName += alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		return fileName + L"." + fileExtenstion;
	}

	static wstring HoneypotNameGenerator::generateRandomFileName(wstring fileExtenstion) {
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

		case 6:
			return FOLDERID_ProgramFiles;
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