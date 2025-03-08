#include "ecg_processor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include <set>

// ----------------- CSV Loading -----------------
std::vector<double> loadECGData(const std::string& filename, int columnIndex) {
    std::vector<double> data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    
    std::string line;
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (firstLine) { // Skip header
            firstLine = false;
            continue;
        }
        
        std::istringstream ss(line);
        std::string token;
        int currentIndex = 0;
        bool valid = false;
        double value = 0.0;
        
        while (std::getline(ss, token, ',')) {
            if (currentIndex == columnIndex) {
                try {
                    token.erase(0, token.find_first_not_of(" \t\n\r"));
                    token.erase(token.find_last_not_of(" \t\n\r") + 1);
                    value = std::stod(token);
                    valid = true;
                } catch (std::exception& e) {
                    valid = false;
                }
                break;
            }
            currentIndex++;
        }
        if (valid) {
            data.push_back(value);
        } else {
            std::cerr << "Warning: Could not parse value on line: " << line << std::endl;
        }
    }
    
    file.close();
    return data;
}

// ----------------- Basic Processing -----------------
double processECGData(const std::vector<double>& data) {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

// ----------------- Moving Average Filter -----------------
std::vector<double> filterECGData(const std::vector<double>& data, int windowSize) {
    std::vector<double> filteredData;
    if (data.empty() || windowSize <= 0) {
        return data;
    }
    int halfWindow = windowSize / 2;
    int dataSize = data.size();
    filteredData.resize(dataSize);
    for (int i = 0; i < dataSize; ++i) {
        double sum = 0.0;
        int count = 0;
        for (int j = i - halfWindow; j <= i + halfWindow; ++j) {
            if (j >= 0 && j < dataSize) {
                sum += data[j];
                ++count;
            }
        }
        filteredData[i] = sum / count;
    }
    return filteredData;
}

// ----------------- Naive Bandpass Filter -----------------
std::vector<double> bandpassFilter(const std::vector<double>& data, int windowSizeLow, int windowSizeHigh) {
    std::vector<double> lowPassed = filterECGData(data, windowSizeLow);
    std::vector<double> highPassed = filterECGData(data, windowSizeHigh);
    std::vector<double> bandpassed(data.size());
    for (size_t i = 0; i < data.size(); i++) {
        bandpassed[i] = lowPassed[i] - highPassed[i];
    }
    return bandpassed;
}

// ----------------- Dynamic Peak Detection with Refractory Period -----------------
std::vector<int> detectPeaksDynamic(const std::vector<double>& data, int windowSize, double multiplier, int refractoryPeriod) {
    std::vector<int> peakIndices;
    int dataSize = data.size();
    int halfWindow = windowSize / 2;
    for (int i = halfWindow; i < dataSize - halfWindow; ++i) {
        double sum = 0.0;
        for (int j = i - halfWindow; j <= i + halfWindow; j++) {
            sum += data[j];
        }
        double localMean = sum / windowSize;
        
        double sqSum = 0.0;
        for (int j = i - halfWindow; j <= i + halfWindow; j++) {
            sqSum += (data[j] - localMean) * (data[j] - localMean);
        }
        double localStd = std::sqrt(sqSum / windowSize);
        double threshold = localMean + multiplier * localStd;
        
        if (data[i] > threshold && data[i] > data[i-1] && data[i] > data[i+1]) {
            if (!peakIndices.empty() && (i - peakIndices.back()) < refractoryPeriod) {
                continue;
            }
            peakIndices.push_back(i);
        }
    }
    return peakIndices;
}

// ----------------- Export Peak Detection Results -----------------
void exportPeakDetectionResults(const std::vector<double>& data, const std::vector<int>& peakIndices, const std::string& outputFile) {
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        std::cerr << "Error: Unable to open output file: " << outputFile << std::endl;
        return;
    }
    // Write CSV header.
    out << "index,ecg_value,is_peak\n";
    
    // Use a set for quick peak lookup.
    std::set<int> peaks(peakIndices.begin(), peakIndices.end());
    for (size_t i = 0; i < data.size(); ++i) {
        out << i << "," << data[i] << ",";
        if (peaks.count(i) > 0) {
            out << "1";
        } else {
            out << "0";
        }
        out << "\n";
    }
    out.close();
    std::cout << "Exported peak detection results to " << outputFile << std::endl;
}

// ----------------- Load Ground Truth Peaks -----------------
std::vector<int> loadGroundTruthPeaks(const std::string& filename) {
    std::vector<int> groundTruth;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open ground truth file: " << filename << std::endl;
        return groundTruth;
    }
    int index;
    while (file >> index) {
        groundTruth.push_back(index);
    }
    file.close();
    return groundTruth;
}
