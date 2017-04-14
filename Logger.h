#pragma once

#include <string>
#include <exception>
#include <windows.h>

using std::string;
using std::exception;

class Logger
{
private:
	string file_log_name;
	int verbosity_level;
	bool enable_logger;

	void print(const string verbosity, const string msg);
	string get_time();
public:
	/* Verbosity level can be: */
	static const int VERBOSE_LEVEL_DEBUG = 0;
	static const int VERBOSE_LEVEL_INFO  = 1;
	static const int VERBOSE_LEVEL_ERROR = 2;

	Logger(string file_log_name, int verbosity_level, bool enable_logger);
	~Logger();

	/* std::string functions */
	void debug(const string msg);
	void info(const string msg);
	void error(const string msg);

	/* Unicode functions */
	static string unicodeToString(LPCWSTR str);
	void debug(LPCWSTR msg);
	void info(LPCWSTR msg);
	void error(LPCWSTR msg);
};

class LoggerFailedToOpenLogException : public exception {};
class LoggerIllegalVerosityLevel : public exception {};

Logger log();

