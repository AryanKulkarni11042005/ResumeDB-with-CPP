#include "Embedding.h"
#include <stdio.h>
#include <nlohmann/json.hpp>
#include <cstdio>
#include <string>
#include <iostream>
using json = nlohmann::json;

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

std::string getEnvVar(const std::string& key){
    const char* val = std::getenv(key.c_str());
    if(val) return std::string(val);

    std::ifstream file(".env");
    std::string line;
    while(std::getline(file, line)){
        auto pos = line.find('=');
        if(pos != std::string::npos){
            std::string k = line.substr(0,pos);
            std::string val = line.substr(pos+1);
            if(k == key) return val;
        }
    }
    return "";
}

std::vector<float> getEmbeddingHF(const std::string& text){
    std::string apiKey = getEnvVar("HF_TOKEN");
    json requestBody;
    requestBody["inputs"] = text;
    std::string body = requestBody.dump();
    CURL* curl = curl_easy_init();
    std::string response;
    
    if(curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "https://router.huggingface.co/hf-inference/models/sentence-transformers/all-MiniLM-L6-v2/pipeline/feature-extraction");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    json parsed = json::parse(response);
    return parsed.get<std::vector<float>>(); 
}
