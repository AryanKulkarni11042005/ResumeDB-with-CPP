#include "ResumeStore.h"
#include "Embedding.h"      // for getEmbedding
#include "PdfUtils.h"       // for extractTextFromPDF
#include "FileUtils.h"      // for getTimeStamp, copyResumeToStorage
#include "Similarity.h"     // for cosineSimilarity
#include <fstream>

ResumeEntry insertResume(const std::string& path, const std::string& originalFileName, 
                          const std::string& companyName, const std::string& role, 
                          const std::string& version, const std::string& status){
    ResumeEntry entry;
    entry.path = path;
    entry.originalFileName = originalFileName;
    entry.companyName = companyName;
    entry.role = role;
    entry.versionName = version;
    entry.status = status;
    entry.dateUploaded = getTimeStamp();
    std::string extractedResumeText = extractTextFromPDF(path);
    entry.embedding = getEmbedding(extractedResumeText);
    std::string storedPath = copyResumeToStorage(entry.path, entry.companyName, entry.role);
    entry.path = storedPath;
    return entry;
}

void ResumeStore::addEntry(ResumeEntry entry){
    entry.id = entries.size();
    entries.push_back(entry);
}

void ResumeStore::saveToFile(const std::string& filePath){
    json j = entries;
    std::ofstream out(filePath);
    out << j.dump(4);
}

void ResumeStore::loadFromFile(const std::string& filePath){
    std::ifstream in(filePath);
    if(!in) return;
    json j;
    in >> j;
    entries = j.get<std::vector<ResumeEntry>>();
    for(int i = 0; i < entries.size(); i++){
        entries[i].id = i;
    }
}

int ResumeStore::search(const std::string& queryText) const {
    std::vector<float> queryEmbedding = getEmbedding(queryText);
    int bestIdx = -1;
    float bestScore = -1.0f;
    for(int i = 0; i < entries.size(); i++){
        float score = cosineSimilarity(queryEmbedding, entries[i].embedding);
        if(score > bestScore){
            bestScore = score;
            bestIdx = i;
        }
    }
    return bestIdx;
}

ResumeEntry ResumeStore::getEntry(int id) const {
    return entries[id];
}

long long ResumeStore::entriesCount() const {
    return entries.size();
}