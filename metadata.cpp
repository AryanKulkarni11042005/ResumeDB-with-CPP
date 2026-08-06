#include <iostream>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

using json = nlohmann::json;

struct ResumeEntry {
    std::string path;              
    std::string originalFileName;  
    std::string companyName;
    std::string role;
    std::string versionName;
    std::string status;
    std::string dateUploaded;
    std::vector<float> embedding;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ResumeEntry, path, originalFileName, companyName, role, versionName, status, dateUploaded, embedding);

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
                                std::string timeStamp = getTimeStamp();
                                std::string storedPath = copyResumeToStorage(entry.path, entry.companyName, entry.role);
                                entry.path = storedPath;
                                return entry;
                            }
int main(){
    ResumeEntry entry = insertResume(
        "/Users/aryankulkarni/Desktop/Company Applications/Resume/Aryan_Kulkarni_Resume_Microsoft.pdf",
        "Aryan_Kulkarni_Resume_Microsoft.pdf",
        "Microsoft",
        "SoftwareEngineer1",
        "More C++ and Cloud",
        "applied"
    );

    std::cout << "Stored at: " << entry.path << std::endl;
    std::cout << "Embedding size: " << entry.embedding.size() << std::endl;

    return 0;
}