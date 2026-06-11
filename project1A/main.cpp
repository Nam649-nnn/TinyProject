#include <iostream>
#include <vector>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "LinearSystem.hpp"
#include "DataUtils.hpp"

int main() {
    std::cout << "========================================================\n";
    std::cout << "PART B: Linear Regression for Relative CPU Performance\n";
    std::cout << "========================================================\n";

    // 1. Load the dataset
    // (Ensure you download 'machine.data' from the UCI page and place it next to your executable)
    std::vector<CPUInstance> dataset = LoadCPUDataset("machine.data");
    
    if (dataset.empty()) {
        std::cerr << "Execution aborted: Data file is empty or missing.\n";
        return 1;
    }
    
    int total_instances = dataset.size();
    std::cout << "Successfully loaded " << total_instances << " instances from dataset.\n";

    // 2. Determine 80% Train / 20% Test split indexes
    int train_size = static_cast<int>(total_instances * 0.80);
    int test_size = total_instances - train_size;
    
    std::cout << "Training instances (80%): " << train_size << "\n";
    std::cout << "Testing instances (20%):  " << test_size << "\n\n";

    // 3. Populate Training Matrix (A_train) and Target Vector (b_train)
    // Dimension: A is (train_size x 6 features) | b is (train_size)
    Matrix A_train(train_size, 6);
    Vector b_train(train_size);
    
    for (int i = 1; i <= train_size; ++i) {
        const auto& instance = dataset[i - 1];
        A_train(i, 1) = instance.myct;
        A_train(i, 2) = instance.mmin;
        A_train(i, 3) = instance.mmax;
        A_train(i, 4) = instance.cach;
        A_train(i, 5) = instance.chmin;
        A_train(i, 6) = instance.chmax;
        
        b_train(i) = instance.prp;
    }

    // 4. Populate Testing Matrix (A_test) and Target Vector (b_test)
    Matrix A_test(test_size, 6);
    Vector b_test(test_size);
    
    for (int i = 1; i <= test_size; ++i) {
        const auto& instance = dataset[train_size + i - 1];
        A_test(i, 1) = instance.myct;
        A_test(i, 2) = instance.mmin;
        A_test(i, 3) = instance.mmax;
        A_test(i, 4) = instance.cach;
        A_test(i, 5) = instance.chmin;
        A_test(i, 6) = instance.chmax;
        
        b_test(i) = instance.prp;
    }

    // 5. Train Linear Regression Model via your NonSquareLinSystem Solver
    std::cout << "--- Training Model Parameters [x1, x2, x3, x4, x5, x6] ---\n";
    
    // We utilize Tikhonov Regularization with a tiny alpha to guarantee numeric inversion stability
    double alpha = 1e-4; 
    NonSquareLinSystem regression_system(A_train, b_train, alpha);
    
    // weights vector containing optimized parameters [x1, x2, x3, x4, x5, x6]
    Vector weights = regression_system.Solve();
    
    std::cout << "Learned weights (Parameters x):\n" << weights << "\n\n";
    
    // 6. Evaluate via Root Mean Square Error (RMSE)
    double train_rmse = CalculateRMSE(A_train, b_train, weights);
    double test_rmse  = CalculateRMSE(A_test, b_test, weights);

    std::cout << "--- Evaluation Statistics ---\n";
    std::cout << "Training Set RMSE: " << train_rmse << "\n";
    std::cout << "Testing Set RMSE:  " << test_rmse << "\n";
    std::cout << "========================================================\n";

    return 0;
}
