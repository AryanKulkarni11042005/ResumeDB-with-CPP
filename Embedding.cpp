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
