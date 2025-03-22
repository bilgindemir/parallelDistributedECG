#ifndef ECG_PROCESSOR_H
#define ECG_PROCESSOR_H

#include <vector>
#include <string>

// Loads ECG data from a CSV file, skipping the header and extracting a specified column.
// Default is column index 2 (for MLII).
std::vector<double> loadECGData(const std::string& filename, int columnIndex = 2);

// Computes the average value of the provided ECG data.
double processECGData(const std::vector<double>& data);

// Applies a simple moving average filter with the specified window size.
std::vector<double> filterECGData(const std::vector<double>& data, int windowSize);

// Applies a naive bandpass filter to isolate the QRS complex.
std::vector<double> bandpassFilter(const std::vector<double>& data, int windowSizeLow, int windowSizeHigh);

// Detects peaks using a dynamic threshold calculated on local signal statistics.
// 'windowSize' defines the local window for computing the threshold,
// 'multiplier' adjusts sensitivity,
// 'refractoryPeriod' (in samples) ignores peaks that occur too close to each other.
std::vector<int> detectPeaksDynamic(const std::vector<double>& data, int windowSize, double multiplier, int refractoryPeriod);

// Exports the ECG data and detected peaks to a CSV file.
// The CSV file will contain: index, ecg_value, is_peak (1 if a peak was detected, else 0).
void exportPeakDetectionResults(const std::vector<double>& data, const std::vector<int>& peakIndices, const std::string& outputFile);

#endif // ECG_PROCESSOR_H
