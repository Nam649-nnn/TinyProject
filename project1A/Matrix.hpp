#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include "Vector.hpp" // Đảm bảo nạp Vector để tính toán nhân Matrix * Vector

class Matrix {
private:
    int mNumRows;       // Số hàng của ma trận
    int mNumCols;       // Số cột của ma trận
    double** mData;     // Con trỏ cấp 2 trỏ tới mảng dữ liệu 2 chiều

public:
    // 1. Các Hàm khởi tạo và Hủy (Constructors & Destructor)
    Matrix();
    Matrix(int numRows, int numCols);
    Matrix(const Matrix& other);
    ~Matrix();

    // 2. Các hàm lấy thông tin (Getters)
    int GetNumberOfRows() const;
    int GetNumberOfCols() const;

    // 3. Toán tử gán (Assignment Operator)
    Matrix& operator=(const Matrix& other);

    // 4. Toán tử chỉ mục 1-based bằng dấu ngoặc đơn ( )
    double& operator()(int i, int j);
    double operator()(int i, int j) const;

    // 5. Các toán tử Unary (Unary Operators)
    Matrix operator+() const;
    Matrix operator-() const;

    // 6. Các toán tử Binary (Binary Operators)
    Matrix operator+(const Matrix& other) const; // Ma trận + Ma trận
    Matrix operator-(const Matrix& other) const; // Ma trận - Ma trận
    
    // Phép nhân (Multiplication)
    Matrix operator*(const Matrix& other) const; // Ma trận * Ma trận
    Vector operator*(const Vector& vec) const;   // Ma trận * Vector
    Matrix operator*(double scalar) const;       // Ma trận * Vô hướng

    // 7. Đại số tuyến tính nâng cao (Advanced Linear Algebra)
    double Determinant() const;                  // Tính định thức (cho ma trận vuông)
    Matrix Inverse() const;                      // Tính ma trận nghịch đảo
    Matrix Transpose() const;                    // Tính ma trận chuyển vị (phục vụ tính Pseudo-Inverse)
    Matrix PseudoInverse() const;                // Tính giả nghịch đảo Moore-Penrose

    // Hàm bạn giúp in ma trận ra màn hình nhanh chóng
    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat);
};

// Toán tử nhân Vô hướng * Ma trận (Đảo vị trí)
Matrix operator*(double scalar, const Matrix& mat);

#endif // MATRIX_HPP