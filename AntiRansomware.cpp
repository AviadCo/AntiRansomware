// AntiRansomware.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>

using std::string;

int main()
{
	string BITCOIN_WALLET = "wallet.dat";
	const string FILE_EXTENSTIONS[] = {
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


    return 0;
}

