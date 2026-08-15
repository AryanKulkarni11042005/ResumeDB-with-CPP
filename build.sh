
#!/bin/bash 

g++ -std=c++17 -I/opt/homebrew/include \
metadata.cpp \
Similarity.cpp \
FileUtils.cpp \
Embedding.cpp \
PdfUtils.cpp \
ResumeStore.cpp \
-o resumedb \
-lcurl 
if [ $? -eq 0 ]; then
    echo "Build succeeded. Running..."
    ./resumedb
else
    echo "Build failed."
fi