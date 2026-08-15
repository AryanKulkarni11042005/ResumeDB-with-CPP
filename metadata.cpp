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
#include "Similarity.h"
#include "FileUtils.h"
#include "PdfUtils.h"
#include "Embedding.h"
#include "ResumeStore.h"

namespace fs = std::filesystem;

using json = nlohmann::json;



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