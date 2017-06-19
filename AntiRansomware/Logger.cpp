#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <codecvt>
#include <locale>
#include <fcntl.h>
#include <io.h>

#include "Logger.h"

using namespace std;

Logger log()
{
	static Logger log = Logger("log.txt", Logger::VERBOSE_LEVEL_DEBUG, true);

	return log;
}

Logger::Logger(string file_log_name, int verbosity_level, bool enable_logger)
{
	this->file_log_name   = file_log_name;
	this->verbosity_level = verbosity_level;
	this->enable_logger   = enable_logger;

	if (verbosity_level < VERBOSE_LEVEL_DEBUG || verbosity_level > VERBOSE_LEVEL_ERROR) {
		cout << get_time() << "Illegal verbosity level: " << verbosity_level << endl;
		throw LoggerIllegalVerosityLevel();
	}

	if (!this->enable_logger) {
		cout << get_time() << "Logger is disabled." << endl;

		return;
	}

	ofstream myfile(this->file_log_name, ios_base::app);
	if (myfile.is_open()) {
		cout << get_time() << "Logger initialized with log file '" << file_log_name << "' and verbosity level " << verbosity_level
			 << endl;

		myfile << get_time() << "This is a Logger file for AntiRandomware" << endl;
		myfile << get_time() << "Logger verbosity is " << verbosity_level << endl;
		myfile.close();
	}
	else {
		throw LoggerFailedToOpenLogException();
	}
}

string Logger::get_time()
{
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %I:%M:%S", &timeinfo);
	return "[ " + string(buffer) + " ] - ";
}

void Logger::print(const string verbosity, const string tag, const string msg)
{
	if (!this->enable_logger) {
		return;
	}

	cout << verbosity << get_time() << tag << ":  " << msg << endl;

	ofstream myfile(this->file_log_name, ios_base::app);
	if (myfile.is_open()) {
		myfile << verbosity << get_time() << tag << ":  " << msg << endl;
		myfile.close();
	}
	else {
		throw LoggerFailedToOpenLogException();
	}
}

void Logger::print(const wstring verbosity, const wstring tag, const wstring msg)
{
	if (!this->enable_logger) {
		return;
	}

	_setmode(_fileno(stdout), _O_U16TEXT);
	wcout << verbosity << stringToWString(get_time()) << tag << L":  " << msg << endl;

	//const std::locale utf8_locale = std::locale(std::locale(),
	//	new std::codecvt_utf8<wchar_t>());

	std::locale mylocale("");

	wofstream myfile(this->file_log_name, ios_base::app);
	if (myfile.is_open()) {
		myfile.imbue(mylocale);
		myfile << verbosity << stringToWString(get_time()) << tag << L":  " << msg << endl;
		myfile.close();
	}
	else {
		throw LoggerFailedToOpenLogException();
	}
}

/* std::string Functions */
void Logger::debug(const string tag, const string msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_DEBUG) {
		print("[DEBUG]", tag, msg);
	}
}

void Logger::info(const string tag, const string msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_INFO) {
		print("[INFO]", tag, msg);
	}
}

void Logger::error(const string tag, const string msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_ERROR) {
		print("[ERROR]", tag, msg);
	}
}

/* std::string Functions End */

/* std::wstring functions */
string Logger::wstringToString(wstring str) {

	return string(str.begin(), str.end());
}

void Logger::debug(const string tag, wstring msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_DEBUG) {
		print(L"[DEBUG]", stringToWString(tag), msg);	
	}
}

void Logger::info(const string tag, wstring msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_INFO) {
		print(L"[INFO]", stringToWString(tag), msg);
	}
}

void Logger::error(const string tag, wstring msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_ERROR) {
		print(L"[ERROR]", stringToWString(tag), msg);
	}
}

/* Unicode functions End */

/* Unicode functions */
string Logger::unicodeToString(LPCWSTR str) {
	wstring ws(str);

	return string(ws.begin(), ws.end());
}

std::wstring Logger::stringToWString(const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());

	return wsTmp;
}

void Logger::debug(const string tag, LPCWSTR msg)
{
	debug(tag, unicodeToString(msg));
}

void Logger::info(const string tag, LPCWSTR msg)
{
	info(tag, unicodeToString(msg));
}

void Logger::error(const string tag, LPCWSTR msg)
{
	error(tag, unicodeToString(msg));
}

/* Unicode functions End */

Logger::~Logger()
{
	/* nothing to do here */
}