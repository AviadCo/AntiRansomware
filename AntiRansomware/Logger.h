#pragma once

#include <windows.h>

#include <string>

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
	void print(const wstring verbosity, const wstring tag, const wstring msg);
	string get_time();
public:
	/* Verbosity level can be: */
	static const int VERBOSE_LEVEL_DEBUG = 0;
	static const int VERBOSE_LEVEL_INFO  = 1;
	static const int VERBOSE_LEVEL_ERROR = 2;

	Logger(string file_log_name, int verbosity_level, bool enable_logger);
	~Logger();

	/************************************************************************/
	/* string functions                                                    */
	/************************************************************************/
	//************************************
	// Method:    debug - print in the log file with log level 'debug'
	// FullName:  Logger::debug
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: string msg - Content of the log message
	//************************************
	void debug(const string tag, const string msg);
	//************************************
	// Method:    info - print in the log file with log level 'info'
	// FullName:  Logger::info
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: string msg - Content of the log message
	//************************************
	void info(const string tag, const string msg);
	//************************************
	// Method:    error - print in the log file with log level 'error'
	// FullName:  Logger::error
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: string msg - Content of the log message
	//************************************
	void error(const string tag, const string msg);

	/************************************************************************/
	/* wstring functions                                                    */
	/************************************************************************/
	static string wstringToString(wstring str);
	//************************************
	// Method:    debug - print in the log file with log level 'debug'
	// FullName:  Logger::debug
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: wstring msg - Content of the log message
	//************************************
	void debug(const string tag, wstring msg);
	//************************************
	// Method:    info - print in the log file with log level 'info'
	// FullName:  Logger::info
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: wstring msg - Content of the log message
	//************************************
	void info(const string tag, wstring msg);
	//************************************
	// Method:    error - print in the log file with log level 'error'
	// FullName:  Logger::error
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: wstring msg - Content of the log message
	//************************************
	void error(const string tag, wstring msg);

	/************************************************************************/
	/* Unicode functions                                                    */
	/************************************************************************/
	static std::wstring stringToWString(const std::string &s);
	static string unicodeToString(LPCWSTR str);
	//************************************
	// Method:    debug - print in the log file with log level 'debug'
	// FullName:  Logger::debug
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: LPCWSTR msg - Content of the log message
	//************************************
	void debug(const string tag, LPCWSTR msg);
	//************************************
	// Method:    info - print in the log file with log level 'info'
	// FullName:  Logger::info
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: LPCWSTR msg - Content of the log message
	//************************************
	void info(const string tag, LPCWSTR msg);
	//************************************
	// Method:    error - print in the log file with log level 'error'
	// FullName:  Logger::error
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: const string tag - Common use: name of the called function (__FUNCTION__)
	// Parameter: LPCWSTR msg - Content of the log message
	//************************************
	void error(const string tag, LPCWSTR msg);
};

class LoggerFailedToOpenLogException : public exception {};
class LoggerIllegalVerosityLevel : public exception {};

Logger log();

