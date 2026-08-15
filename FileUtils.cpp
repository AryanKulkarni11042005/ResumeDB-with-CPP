#include "FileUtils.h"


#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string getTimeStamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y%m%d_%H%M%S");
    return oss.str();
}

std::string buildFileName(const std::string& company, const std::string& role){
    return company + "_" + role + "_" + getTimeStamp() + ".pdf";
}

std::string copyResumeToStorage(const std::string& sourcePath, const std::string& company, const std::string& role){
    std::string storageDir = "resume_storage/";
    fs::create_directories(storageDir);

    std::string fileName = buildFileName(company, role);
    std::string destPath = storageDir + fileName;
    fs::copy_file(sourcePath, destPath);
    return destPath; 
}

std::string stripQuotes(const std::string& s) {
    if (s.size() >= 2) {
        char first = s.front();
        char last = s.back();
        if ((first == '"' && last == '"') || (first == '\'' && last == '\'')) {
            return s.substr(1, s.size() - 2);
        }
    }
    return s;
}