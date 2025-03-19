#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <experimental/filesystem>
#include "ecg_processor.h"

namespace fs = std::experimental::filesystem;  // or std::filesystem if supported

int main() {
    std::string directoryPath = "../data/mitbih_all/";
    
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".csv") {
            std::string filename = entry.path().string();
            std::cout << "\nProcessing file: " << filename << std::endl;
            
            // Start timing for this file
            auto startTime = std::chrono::high_resolution_clock::now();
            
            std::vector<double> ecgData;
            try {
                ecgData = loadECGData(filename, 2);
            } catch (std::exception& ex) {
                std::cerr << "Error loading " << filename << ": " << ex.what() << std::endl;
                continue;
            }
            
            std::cout << "Loaded " << ecgData.size() << " data points." << std::endl;
            double rawAverage = processECGData(ecgData);
            std::cout << "Average raw ECG signal value: " << rawAverage << std::endl;
            
            int windowSize = 10;
            std::vector<double> filteredData = filterECGData(ecgData, windowSize);
            std::cout << "Applied moving average filter with window size " << windowSize << "." << std::endl;
            
            int windowSizeLow = 20;
            int windowSizeHigh = 5;
            std::vector<double> bandpassedData = bandpassFilter(ecgData, windowSizeLow, windowSizeHigh);
            std::cout << "Applied naive bandpass filter with window sizes " << windowSizeLow 
                      << " (low-pass) and " << windowSizeHigh << " (high-pass)." << std::endl;
            
            int dynamicWindowSize = 100;
            double multiplier = 2.0;
            int refractoryPeriod = 200;
            std::vector<int> dynamicPeakIndices = detectPeaksDynamic(bandpassedData, dynamicWindowSize, multiplier, refractoryPeriod);
            std::cout << "Detected " << dynamicPeakIndices.size() 
                      << " peaks in the bandpassed ECG data using dynamic thresholding." << std::endl;
            
            // Export results
            std::string stem = entry.path().stem().string();
            std::string outputCSV = directoryPath + stem + "_results.csv";
            exportPeakDetectionResults(ecgData, dynamicPeakIndices, outputCSV);
            
            // End timing and report
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "Processing time for " << stem << ": " << duration.count() << " ms" << std::endl;
        }
    }
    
    return 0;
}