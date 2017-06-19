#pragma once

#include <string>
#include <vector>

namespace StringFunctions
{
	/**
	* Splits a string up into a series of tokens (output as a string vector).
	*
	* @param    sourceStr   The string to split up.
	*
	* @param    delimiter   What to delimit the source string by.
	*
	* @param    ignoreEmpty    Whether or not to ignore empty tokens.
	*                               (usually created by 2 or more immediately
	*                               adjacent delimiters)
	*
	* @return   A string vector containing a series of ordered tokens.
	*/
	std::vector<std::wstring> split(const std::wstring &sourceStr, const std::wstring &delimiter, bool ignoreEmpty);

	/**
	* Splits the param string up into a series of parameters (output as a string vector).
	*
	* @param    sourceStr   The string to split up.
	*
	* @return   A string vector containing a series of parameters.
	*/
	std::vector<std::wstring> splitParam(const std::wstring &sourceStr);
};

