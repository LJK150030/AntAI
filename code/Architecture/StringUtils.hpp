#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
const std::string Stringf(const char* format, ...);
const std::string Stringf(int max_length, const char* format, ...);
std::vector<std::string> SplitStringOnDelimiter(const std::string& original_string, char delimiter);
