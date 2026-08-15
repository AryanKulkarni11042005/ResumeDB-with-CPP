#pragma once
#include <string>

std::string getTimeStamp();
std::string buildFileName(const std::string& company, const std::string& role);
std::string copyResumeToStorage(const std::string& sourcePath, const std::string& company, const std::string& role);
std::string stripQuotes(const std::string& s);


