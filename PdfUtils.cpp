#include "PdfUtils.h"
#include <string>
#include <fstream>
#include <iostream>


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


