#ifndef DATAUTILS_HPP
#define DATAUTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include "Matrix.hpp"
#include "Vector.hpp"

// Structure to hold a raw data row (ignoring vendor and model names strings)
struct CPUInstance {
    double myct;
    double mmin;
    double mmax;
    double cach;
    double chmin;
    double chmax;
    double prp; // Target Goal
};

// Function to parse the machine.data file from UCI
std::vector<CPUInstance> LoadCPUDataset(const std::string& filename) {
    std::vector<CPUInstance> dataset;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        std::cerr << "Please download 'machine.data' from UCI and place it in the working directory.\n";
        return dataset;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> row_tokens;
        
        while (std::getline(ss, token, ',')) {
            row_tokens.push_back(token);
        }
        
        // Ensure row has at least 9 elements (Vendor, Model, + 7 numeric fields)
        if (row_tokens.size() >= 9) {
            CPUInstance instance;
            try {
                instance.myct  = std::stod(row_tokens[2]);
                instance.mmin  = std::stod(row_tokens[3]);
                instance.mmax  = std::stod(row_tokens[4]);
                instance.cach  = std::stod(row_tokens[5]);
                instance.chmin = std::stod(row_tokens[6]);
                instance.chmax = std::stod(row_tokens[7]);
                instance.prp   = std::stod(row_tokens[8]); // Goal field
                dataset.push_back(instance);
            } catch (...) {
                // Skip lines that fail parsing due to formatting anomalies
                continue;
            }
        }
    }
    file.close();
    return dataset;
}

// Function to calculate Root Mean Square Error (RMSE)
double CalculateRMSE(const Matrix& X, const Vector& y, const Vector& weights) {
    int n = X.GetNumberOfRows();
    double squared_error_sum = 0.0;
    
    // Predict: y_pred = X * weights
    Vector y_pred = X * weights;
    
    for (int i = 0; i < n; ++i) {
        // Mapping internal 0-based Vector index
        double error = y_pred[i] - y[i];
        squared_error_sum += error * error;
    }
    
    return std::sqrt(squared_error_sum / n);
}

#endif // DATAUTILS_HPP