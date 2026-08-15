#pragma once 

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct ResumeEntry {
    int id;
    std::string path;              
    std::string originalFileName;  
    std::string companyName;
    std::string role;
    std::string versionName;
    std::string status;
    std::string dateUploaded;
    std::vector<float> embedding;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResumeEntry, id, path, originalFileName, companyName, role, versionName, status, dateUploaded, embedding);
};

ResumeEntry insertResume(const std::string& path, const std::string& originalFileName, 
                          const std::string& companyName, const std::string& role, 
                          const std::string& version, const std::string& status);

class ResumeStore {
public:
    void addEntry(ResumeEntry entry);
    void saveToFile(const std::string& filePath);
    void loadFromFile(const std::string& filePath);
    int search(const std::string& queryText) const;
    ResumeEntry getEntry(int id) const;
    long long entriesCount() const;

private: 
    std::vector<ResumeEntry> entries;
};