#pragma once

#include <Windows.h>
#include <KnownFolders.h>
#include <shlobj.h>
#include <vector>
#include <list>
#include <exception>

#include "HoneypotNameGenerator.h"
#include "Logger.h"

#define MAX_FILENAME_LENGTH 50
#define DEFAULT_NUMBER_OF_HONEYPOTS_NAME 20
#define DEFAULT_NUMBER_OF_DOLFER_IDS 7

static const string BITCOIN_WALLET = "wallet.dat";
static const std::vector<string> FILE_EXTENSTIONS = {
	"mid", "wma", "flv", "mkv", "mov", "avi", "asf", "mpeg", "vob",
	"mpg", "wmv", "fla", "swf", "wav", "qcow2", "vdi", "vmdk",
	"vmx", "gpg", "aes", "ARC", "PAQ", "tar.bz2", "tbk", "bak", "tar",
	"tgz", "rar", "zip", "djv", "djvu", "svg", "bmp", "png", "gif", "raw",
	"cgm", "jpeg", "jpg", "tif", "tiff", "NEF", "psd", "cmd", "bat", "class",
	"jar", "java", "asp", "brd", "sch", "dch", "dip", "vbs", "asm", "pas", "cpp",
	"php", "ldf", "mdf", "ibd", "MYI", "MYD", "frm", "odb", "dbf", "mdb",
	"sql", "SQLITEDB", "SQLITE3", "asc", "lay6", "lay", "ms11", "sldm",
	"sldx", "ppsm", "ppsx", "ppam", "docb", "mml", "sxm", "ofg", "odg",
	"uop", "potx", "potm", "pptx", "pptm", "std", "sxd", "pot", "pps",
	"sti", "sxi", "otp", "odp", "wks", "xltx", "xltm", "xlsx", "xlsm",
	"slk", "xlw", "xlt", "xlm", "xlc", "dif", "stc", "sxc", "ots",
	"ods", "hwp", "dotm", "dotx", "docm", "docx", "DOT", "max", "xml",
	"txt", "CSV", "uot", "RTF", "pdf", "XLS", "PPT", "stw", "sxw",
	"ott", "odt", "DOC" , "pem", "csr", "crt", "kev",
};

class HoneypotsNameGeneratorFailure : public exception {};

class HoneypotNameGenerator
{
private:

public:
	HoneypotNameGenerator();
	~HoneypotNameGenerator();
	static string HoneypotNameGenerator::getRandomFileExtenstion()
	{
		return FILE_EXTENSTIONS[rand() % (FILE_EXTENSTIONS.size() - 1)];
	}

	static string HoneypotNameGenerator::getFileExtenstion(unsigned int extension)
	{
		if (extension >= FILE_EXTENSTIONS.size()) {
			log().error("Failed to get file extension with index " + extension);

			throw HoneypotsNameGeneratorFailure();
		}

		return FILE_EXTENSTIONS[extension];
	}

	static string HoneypotNameGenerator::generateRandomFileName(unsigned int len, string fileExtenstion) {
		string fileName;

		if (len > MAX_FILENAME_LENGTH) {
			log().error("Failed to generate random file name, file name length is bigger than " + MAX_FILENAME_LENGTH);

			throw HoneypotsNameGeneratorFailure();
		}

		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

		for (unsigned int i = 0; i < len; ++i) {
			fileName += alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		return fileName + "." + fileExtenstion;
	}

	static string HoneypotNameGenerator::generateRandomFileName(string fileExtenstion) {
		return generateRandomFileName(rand() % MAX_FILENAME_LENGTH, fileExtenstion);
	}

	static string HoneypotNameGenerator::getDirectoryPath(REFKNOWNFOLDERID rdid)
	{
		PWSTR path = NULL;
		string pathStr = "";

		if (SUCCEEDED(SHGetKnownFolderPath(rdid, 0, NULL, &path))) {
			pathStr = Logger::unicodeToString(path);

			log().debug("Succeeded to get folder path: " + pathStr);

			/* must call free on SHGetKnownFolderPath success */
			CoTaskMemFree(path);
		}
		else {
			log().error("Failed to get folder path to create Honeypot name.");

			throw HoneypotsNameGeneratorFailure();
		}

		return pathStr + "\\";
	}

	static string HoneypotNameGenerator::getFullFileName(string fileName, REFKNOWNFOLDERID rdid)
	{
		return getDirectoryPath(rdid) + fileName;
	}

	static string HoneypotNameGenerator::getRandomFullFileName(string fileExtenstion, REFKNOWNFOLDERID rdid)
	{
		return getDirectoryPath(rdid) + generateRandomFileName(fileExtenstion);
	}

	static string HoneypotNameGenerator::getRandomFullFileName(REFKNOWNFOLDERID rdid)
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

	static std::list<string> HoneypotNameGenerator::createFullFileNames(unsigned int num_of_files)
	{
		std::list<string> filesList = std::list<string>();

		for (unsigned int i = 0; i < num_of_files; ++i) {
			string newFileName = getRandomFullFileName(getRandomFolderID());

			log().info("New Honeypot name created: " + newFileName);

			filesList.push_back(newFileName);
		}

		return filesList;
	}

	static std::list<string> HoneypotNameGenerator::createFullFileNames()
	{
		return createFullFileNames(DEFAULT_NUMBER_OF_HONEYPOTS_NAME);
	}

};