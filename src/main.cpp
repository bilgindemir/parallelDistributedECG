#include <iostream>
#include <vector>
#include <string>
#include <experimental/filesystem>
#include <omp.h>
#include <chrono>
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

    if (files.empty()) {
        std::cerr << "No CSV files found in " << directoryPath << std::endl;
        return 1;
    }

    // Define the threads counts to test (e.g., {1, 2, 3, 4})
    std::vector<int> threadCounts = {1, 2, 3, 4, 5, 6, 7, 8, 16, 32};

    // Loop over each thread count to measure performance
    for (int numThreads : threadCounts) {
        // Set the number of threads for OpenMP
        omp_set_num_threads(numThreads);
        std::cout << "\n=== Processing with " << numThreads << " threads ===\n";

        // Start overall timer for this run
        auto overallStart = std::chrono::high_resolution_clock::now();

        // Process files in parallel using OpenMP
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < files.size(); ++i) {
            std::string filename = files[i].string();
            // Print the file being processed (messages may interleave)
            std::cout << "Processing file: " << filename << std::endl;

            std::vector<double> ecgData;
            try {
                // Load MLII data from column index 1.
                ecgData = loadECGData(filename, 1);
            } catch (std::exception& ex) {
                // Print errors without synchronization
                std::cerr << "Error loading " << filename << ": " << ex.what() << std::endl;
                continue;
            }

            // Process the ECG data sequentially for this file:
            double rawAverage = processECGData(ecgData);
            int windowSize = 10;
            std::vector<double> filteredData = filterECGData(ecgData, windowSize);
            int windowSizeLow = 20;
            int windowSizeHigh = 5;
            std::vector<double> bandpassedData = bandpassFilter(ecgData, windowSizeLow, windowSizeHigh);
            int dynamicWindowSize = 100;
            double multiplier = 2.0;
            int refractoryPeriod = 200;
            std::vector<int> dynamicPeakIndices = detectPeaksDynamic(bandpassedData, dynamicWindowSize, multiplier, refractoryPeriod);

            // Optionally export results if needed
            std::string stem = files[i].stem().string();
            std::string outputCSV = directoryPath + stem + "_results.csv";
            exportPeakDetectionResults(ecgData, dynamicPeakIndices, outputCSV);
        }

        // End overall timer for this thread count run
        auto overallEnd = std::chrono::high_resolution_clock::now();
        auto overallDuration = std::chrono::duration_cast<std::chrono::milliseconds>(overallEnd - overallStart);
        std::cout << "Total processing time with " << numThreads << " threads: "
                  << overallDuration.count() << " ms" << std::endl;
    }

    return 0;
}