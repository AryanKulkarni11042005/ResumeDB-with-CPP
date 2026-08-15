#pragma once
#include <string>
#include <cstdio>
#include <curl/curl.h>
#include <vector>
#include <fstream>
#include <cstdlib>

size_t WriteCallBack(void* contents, size_t size, size_t nmemb, std::string* userp);
std::string callOllama(const std::string& jsonBody);
std::vector<float> getEmbedding(const std::string& text);
std::string getEnvVar(const std::string& key);
std::vector<float> getEmbeddingHF(const std::string& text);
