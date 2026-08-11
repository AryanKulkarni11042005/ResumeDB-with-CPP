#include <iostream>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <cmath>

namespace fs = std::filesystem;

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
size_t WriteCallBack(void* contents, size_t size, size_t nmemb, std::string* userp){
    size_t totalSize = size*nmemb;
    userp->append((char* )contents, totalSize);
    return totalSize;
}
std::string callOllama(const std::string& jsonBody){
    CURL* curl = curl_easy_init();
    std::string response;
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:11434/api/embeddings");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,jsonBody.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,WriteCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return response;
}
std::vector<float> getEmbedding(const std::string& text){
    json requestBody;
    requestBody["model"] = "nomic-embed-text";
    requestBody["prompt"] = text;
    std::string body = requestBody.dump();  
    std::string rawResponse = callOllama(body);
    json parsed = json::parse(rawResponse);
    std::vector<float> embedding = parsed["embedding"].get<std::vector<float>>();

    return embedding;
}
std::string extractTextFromPDF(const std::string& pdfPath){
    std::string command = "pdftotext \"" + pdfPath + "\" -";
    FILE *pipe = popen(command.c_str(), "r");
    if(!pipe){
        throw std::runtime_error("popen failed");
    }
    std::string result;
    char buffer[256];
    while(fgets(buffer, sizeof(buffer), pipe) != nullptr){
        result += buffer;
    }
    pclose(pipe);
    return result;
}
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
/// MAIN STARTS HERE


float dotProduct(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0;
    for (int i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

float magnitude(const std::vector<float>& v) {
    float sum = 0;
    for (int i = 0; i < v.size(); i++) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    return dotProduct(a, b) / (magnitude(a) * magnitude(b));
}

ResumeEntry insertResume(const std::string& path,const std::string& originalFileName, 
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

class ResumeStore{
public:
    void addEntry(ResumeEntry& entry){
        entry.id = entries.size();
        entries.push_back(entry);
    }
    void saveToFile(const std::string& filePath){
        json j = entries;
        std::ofstream out(filePath);
        out << j.dump(4);
    }
    void loadFromFile(const std::string& filePath){
        std::ifstream in(filePath);
        if(!in) return;
        json j;
        in >> j;
        entries = j.get<std::vector<ResumeEntry>>();
        for(int i = 0 ; i< entries.size(); i++){
            entries[i].id = i;
        }
    }
    int search(const std::string& queryText) const {
        std::vector<float> queryEmbedding = getEmbedding(queryText);
        int bestIdx = - 1;
        float bestScore = -1.0f;
        for(int i = 0; i<entries.size();i++){
            float score = cosineSimilarity(queryEmbedding, entries[i].embedding);
            if(score>bestScore){
                bestScore = score;
                bestIdx = i;
            }
        }
        return bestIdx;
    }
    ResumeEntry getEntry(int id) const {
        for(const auto& entry: entries){
            if(entry.id == id){
                return entry;
            }
        }
        throw std::runtime_error("No Resume found for this ID");
    }
    long long entriesCount() const{
        return entries.size();
    }

    
private: 
    std::vector<ResumeEntry> entries;
};
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
int main(){
    ResumeStore store;
    store.loadFromFile("resume_db.json");

    while (true) {
        std::cout << "\n1. Insert Resume\n2. Search\n3. Exit\nChoice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();   // important — explained below

        switch (choice) {
            case 1: {
                std::string path, originalName, company, role, version, status;

                std::cout << "Resume file path: ";
                std::getline(std::cin, path);
                path = stripQuotes(path);

                std::cout << "Original filename: ";
                std::getline(std::cin, originalName);

                std::cout << "Company: ";
                std::getline(std::cin, company);

                std::cout << "Role: ";
                std::getline(std::cin, role);

                std::cout << "Version name: ";
                std::getline(std::cin, version);

                std::cout << "Status: ";
                std::getline(std::cin, status);

                ResumeEntry entry = insertResume(path, originalName, company, role, version, status);
                store.addEntry(entry);
                store.saveToFile("resume_db.json");
                std::cout << "Saved. Total resumes stored: " << store.entriesCount() << std::endl;
                break;
            }
            case 2: {
                std::cout << "Search for something: ";
                std::string query;
                std::getline(std::cin, query);

                int idx = store.search(query);
                if (idx != -1) {
                    ResumeEntry match = store.getEntry(idx);
                    std::cout << "Best match: " << match.companyName << " - " << match.role << " - " << match.path << std::endl;
                } else {
                    std::cout << "No resumes stored yet." << std::endl;
                }
                break;
            }
            case 3:
                return 0;
            default:
                std::cout << "Invalid choice." << std::endl;
        }
    }
}