#include "Similarity.h"

#include <cmath>

float dotProduct(const std::vector<float>& a, const std::vector<float>& b){
    float sum = 0;
    for(int i = 0; i<a.size(); i++) sum += a[i] * b[i];
    return sum;
}

float magnitude(const std::vector<float>& v){
    float sum = 0;
    for (int i = 0; i < v.size(); i++) {
        sum += v[i] * v[i];
    }
    return sqrt(sum);
}

float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b){
    return dotProduct(a, b) / (magnitude(a) * magnitude(b));
}
