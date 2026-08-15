
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y g++ cmake \
 g++ \
 cmake \
 libcurl4-openssl-dev \
 nlohmann-json3-dev \
 poppler-utils 

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. && make

CMD ["./build/resumedb"]
