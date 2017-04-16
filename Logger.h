#pragma once

#include <string>
#include <exception>
#include <windows.h>

using std::string;
using std::wstring;
using std::exception;

class Logger
{
private:
	string file_log_name;
	int verbosity_level;
	bool enable_logger;

	void print(const string verbosity, const string tag, const string msg);
	string get_time();
public:
	/* Verbosity level can be: */
	static const int VERBOSE_LEVEL_DEBUG = 0;
	static const int VERBOSE_LEVEL_INFO  = 1;
	static const int VERBOSE_LEVEL_ERROR = 2;

	Logger(string file_log_name, int verbosity_level, bool enable_logger);
	~Logger();

	/* std::string functions */
	void debug(const string tag, const string msg);
	void info(const string tag, const string msg);
	void error(const string tag, const string msg);

	/* wstring functions */
	static string wstringToString(wstring str);
	void debug(const string tag, wstring msg);
	void info(const string tag, wstring msg);
	void error(const string tag, wstring msg);

	/* Unicode functions */
	static string unicodeToString(LPCWSTR str);
	void debug(const string tag, LPCWSTR msg);
	void info(const string tag, LPCWSTR msg);
	void error(const string tag, LPCWSTR msg);
};

class LoggerFailedToOpenLogException : public exception {};
class LoggerIllegalVerosityLevel : public exception {};

Logger log();

