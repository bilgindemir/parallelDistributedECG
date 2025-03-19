#include <iostream>
#include <vector>
#include <string>
#include <experimental/filesystem>
#include <omp.h>
#include "ecg_processor.h"

namespace fs = std::experimental::filesystem;

int main() {
    std::string directoryPath = "../data/mitbih_all/";
    std::vector<fs::path> files;

    // Collect only CSV files that are not result files (do not contain "_results" in their stem)
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".csv" &&
            entry.path().stem().string().find("_results") == std::string::npos) {
            files.push_back(entry.path());
        }
    }

    // Process files in parallel using OpenMP
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < files.size(); ++i) {
        std::string filename = files[i].string();
        std::cout << "\nProcessing file: " << filename << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<double> ecgData;
        try {
            // Load MLII data from column index 2.
            ecgData = loadECGData(filename, 2);
        } catch (std::exception& ex) {
            #pragma omp critical
            {
                std::cerr << "Error loading " << filename << ": " << ex.what() << std::endl;
            }
            continue;
        }
        
        #pragma omp critical
        std::cout << "Loaded " << ecgData.size() << " data points." << std::endl;
        
        double rawAverage = processECGData(ecgData);
        #pragma omp critical
        std::cout << "Average raw ECG signal value: " << rawAverage << std::endl;
        
        int windowSize = 10;
        std::vector<double> filteredData = filterECGData(ecgData, windowSize);
        #pragma omp critical
        std::cout << "Applied moving average filter with window size " << windowSize << "." << std::endl;
        
        int windowSizeLow = 20;
        int windowSizeHigh = 5;
        std::vector<double> bandpassedData = bandpassFilter(ecgData, windowSizeLow, windowSizeHigh);
        #pragma omp critical
        std::cout << "Applied naive bandpass filter with window sizes " << windowSizeLow 
                  << " (low-pass) and " << windowSizeHigh << " (high-pass)." << std::endl;
        
        int dynamicWindowSize = 100;
        double multiplier = 2.0;
        int refractoryPeriod = 200;
        std::vector<int> dynamicPeakIndices = detectPeaksDynamic(bandpassedData, dynamicWindowSize, multiplier, refractoryPeriod);
        #pragma omp critical
        std::cout << "Detected " << dynamicPeakIndices.size() 
                  << " peaks in the bandpassed ECG data using dynamic thresholding." << std::endl;
        
        // Construct output filename
        std::string stem = files[i].stem().string();
        std::string outputCSV = directoryPath + stem + "_results.csv";
        exportPeakDetectionResults(ecgData, dynamicPeakIndices, outputCSV);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        #pragma omp critical
        std::cout << "Processing time for " << stem << ": " << duration.count() << " ms" << std::endl;
    }
    
    return 0;
}
