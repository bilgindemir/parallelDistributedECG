#include <iostream>
#include <vector>
#include "ecg_processor.h"

int main() {
    // Path to the MIT-BIH ECG CSV file.
    std::string filename = "../data/mitbih_sample.csv";
    
    std::cout << "Loading ECG data from " << filename << " ..." << std::endl;
    std::vector<double> ecgData;
    try {
        // Load MLII data from column index 2.
        ecgData = loadECGData(filename, 2);
    } catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    
    std::cout << "Loaded " << ecgData.size() << " data points." << std::endl;
    
    // Process raw ECG data.
    double rawAverage = processECGData(ecgData);
    std::cout << "Average raw ECG signal value: " << rawAverage << std::endl;
    
    // Apply a moving average filter for basic smoothing.
    int windowSize = 10;
    std::vector<double> filteredData = filterECGData(ecgData, windowSize);
    std::cout << "Applied moving average filter with window size " << windowSize << "." << std::endl;
    
    // Apply a naive bandpass filter.
    int windowSizeLow = 20;
    int windowSizeHigh = 5;
    std::vector<double> bandpassedData = bandpassFilter(ecgData, windowSizeLow, windowSizeHigh);
    std::cout << "Applied naive bandpass filter with window sizes " << windowSizeLow 
              << " (low-pass) and " << windowSizeHigh << " (high-pass)." << std::endl;
    
    // Dynamic peak detection with refined parameters.
    int dynamicWindowSize = 100;  // Adjust based on local ECG characteristics.
    double multiplier = 2.0;        // Increase to reduce sensitivity.
    int refractoryPeriod = 200;     // In samples (adjust based on sampling rate).
    std::vector<int> dynamicPeakIndices = detectPeaksDynamic(bandpassedData, dynamicWindowSize, multiplier, refractoryPeriod);
    std::cout << "Detected " << dynamicPeakIndices.size() 
              << " peaks in the bandpassed ECG data using dynamic thresholding." << std::endl;
    
    // Export the results for visualization.
    std::string outputCSV = "../data/peak_detection_results.csv";
    exportPeakDetectionResults(ecgData, dynamicPeakIndices, outputCSV);
        
    return 0;
}
