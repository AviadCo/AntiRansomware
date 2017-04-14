#include "stdafx.h"
#include "Logger.h"

#include <iostream>
#include <fstream>
#include <ctime>

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

void Logger::print(const string verbosity, const string msg)
{
	if (!this->enable_logger) {
		return;
	}

	ofstream myfile(this->file_log_name, ios_base::app);
	if (myfile.is_open()) {
		myfile << verbosity << get_time() << msg << endl;
		myfile.close();
	}
	else {
		throw LoggerFailedToOpenLogException();
	}
}

void Logger::debug(const string msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_DEBUG) {
		print("[DEBUG]", msg);
	}
}

void Logger::info(const string msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_INFO) {
		print("[INFO]", msg);
	}
}

void Logger::error(const string msg)
{
	if (this->verbosity_level <= VERBOSE_LEVEL_ERROR) {
		print("[ERROR]", msg);
	}
}

Logger::~Logger()
{
	/* nothing to do here */
}
