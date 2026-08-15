# ResumeDB

A CLI tool to manage placement-season resumes: insert a PDF, and it extracts 
the text, generates a semantic embedding (via Hugging Face's Inference API), 
stores the file with metadata (company, role, version, status), and lets you 
search your resumes by meaning — not just filename.

---

# Windows Setup

## 1. Install a C++ compiler and CMake
- Install Visual Studio (Community edition is free) with the "Desktop 
  development with C++" workload, which includes MSVC and a compatible 
  toolchain — OR install MinGW-w64 g++ if you prefer.
- Install CMake from cmake.org/download and add it to your PATH during setup.

## 2. Install vcpkg (for libcurl and nlohmann-json)
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install curl nlohmann-json
```

## 3. Install Poppler (for pdftotext)
Download a Windows build of Poppler (e.g. from 
github.com/oschwartz10612/poppler-windows/releases), extract it, and add its 
`bin` folder to your system PATH so `pdftotext` is runnable from any terminal.

## 4. Get a free Hugging Face API token
Sign up at huggingface.co → Settings → Access Tokens → create a new token 
(read access is enough).

## 5. Clone the repo
```bash
git clone https://github.com/AryanKulkarni11042005/ResumeDB-with-CPP.git
cd ResumeDB-with-CPP
```
## 6. Set your API token

Create a `.env` file in the project root:

HF_TOKEN=your_huggingface_token_here

## 7. Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```
## 8. Run

From the project root (not the `build` folder, so `.env` is found correctly):
```bash
.\build\resumedb.exe
```

## 9. Using it

You'll see a menu:
1. Insert Resume
2. Search
3. Exit

When asked for a resume path, use your real Windows path, e.g. 
C:\Users\You\Downloads\resume.pdf

---

# Mac / Linux Setup

## 1. Install prerequisites

Mac (via Homebrew):
```bash
brew install cmake curl nlohmann-json poppler
```
Linux (Debian/Ubuntu):
```bash
sudo apt-get update
sudo apt-get install -y g++ cmake libcurl4-openssl-dev nlohmann-json3-dev poppler-utils
```
## 2. Get a free Hugging Face API token
Sign up at huggingface.co → Settings → Access Tokens → create a new token 
(read access is enough).

## 3. Clone the repo
```bash
git clone https://github.com/AryanKulkarni11042005/ResumeDB-with-CPP.git
cd ResumeDB-with-CPP
```

## 4. Set your API token

Create a `.env` file in the project root:

HF_TOKEN=your_huggingface_token_here

## 5. Build
```bash
mkdir build
cd build
cmake ..
make
```
## 6. Run

From the project root (not the `build` folder, so `.env` is found correctly):
```bash
./build/resumedb
```

## 7. Using it

You'll see a menu:
1. Insert Resume
2. Search
3. Exit

When asked for a resume path, use your real Mac/Linux path, e.g. 
/Users/you/Downloads/resume.pdf
