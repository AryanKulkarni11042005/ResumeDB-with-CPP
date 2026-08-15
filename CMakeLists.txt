cmake_minimum_required(VERSION 3.10)
project(ResumeDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Homebrew include path (Apple Silicon)
include_directories(/opt/homebrew/include)

# Find libcurl
find_package(CURL REQUIRED)

add_executable(resumedb
    metadata.cpp
    Similarity.cpp
    FileUtils.cpp
    Embedding.cpp
    PdfUtils.cpp
    ResumeStore.cpp
)

target_link_libraries(resumedb PRIVATE CURL::libcurl)