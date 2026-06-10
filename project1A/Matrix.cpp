#include "Matrix.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>

// --- 1. CONSTRUCTORS & DESTRUCTOR ---
Matrix::Matrix() : mNumRows(0), mNumCols(0), mData(nullptr) {}

Matrix::Matrix(int numRows, int numCols) : mNumRows(numRows), mNumCols(numCols) {
    assert(numRows > 0 && numCols > 0);
    
    // Cấp phát động mảng con trỏ quản lý các dòng
    mData = new double*[mNumRows];
    for (int i = 0; i < mNumRows; ++i) {
        mData[i] = new double[mNumCols];
        for (int j = 0; j < mNumCols; ++j) {
            mData[i][j] = 0.0; // Khởi tạo bằng 0 theo yêu cầu đề bài
        }
    }
}

Matrix::Matrix(const Matrix& other) : mNumRows(other.mNumRows), mNumCols(other.mNumCols) {
    mData = new double*[mNumRows];
    for (int i = 0; i < mNumRows; ++i) {
        mData[i] = new double[mNumCols];
        for (int j = 0; j < mNumCols; ++j) {
            mData[i][j] = other.mData[i][j];
        }
    }
}

Matrix::~Matrix() {
    if (mData != nullptr) {
        for (int i = 0; i < mNumRows; ++i) {
            delete[] mData[i]; // Giải phóng bộ nhớ từng dòng trước
        }
        delete[] mData;        // Giải phóng mảng con trỏ quản lý dòng sau cùng
    }
}

// --- 2. GETTERS ---
int Matrix::GetNumberOfRows() const { return mNumRows; }
int Matrix::GetNumberOfCols() const { return mNumCols; }

// --- 3. ASSIGNMENT OPERATOR ---
Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        // Giải phóng bộ nhớ cũ
        if (mData != nullptr) {
            for (int i = 0; i < mNumRows; ++i) {
                delete[] mData[i];
            }
            delete[] mData;
        }

        // Sao chép thông số và cấp phát mới
        mNumRows = other.mNumRows;
        mNumCols = other.mNumCols;
        mData = new double*[mNumRows];
        for (int i = 0; i < mNumRows; ++i) {
            mData[i] = new double[mNumCols];
            for (int j = 0; j < mNumCols; ++j) {
                mData[i][j] = other.mData[i][j];
            }
        }
    }
    return *this;
}

// --- 4. 1-BASED INDEXING OPERATOR ---
double& Matrix::operator()(int i, int j) {
    assert(i >= 1 && i <= mNumRows);
    assert(j >= 1 && j <= mNumCols);
    return mData[i - 1][j - 1]; // Ánh xạ từ 1-based sang 0-based nội bộ
}

double Matrix::operator()(int i, int j) const {
    assert(i >= 1 && i <= mNumRows);
    assert(j >= 1 && j <= mNumCols);
    return mData[i - 1][j - 1];
}

// --- 5. UNARY OPERATORS ---
Matrix Matrix::operator+() const { return *this; }

Matrix Matrix::operator-() const {
    Matrix temp(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i) {
        for (int j = 0; j < mNumCols; ++j) {
            temp.mData[i][j] = -mData[i][j];
        }
    }
    return temp;
}

// --- 6. BINARY OPERATORS ---
Matrix Matrix::operator+(const Matrix& other) const {
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    Matrix temp(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i) {
        for (int j = 0; j < mNumCols; ++j) {
            temp.mData[i][j] = mData[i][j] + other.mData[i][j];
        }
    }
    return temp;
}

Matrix Matrix::operator-(const Matrix& other) const {
    assert(mNumRows == other.mNumRows && mNumCols == other.mNumCols);
    Matrix temp(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i) {
        for (int j = 0; j < mNumCols; ++j) {
            temp.mData[i][j] = mData[i][j] - other.mData[i][j];
        }
    }
    return temp;
}

// Ma trận * Ma trận
Matrix Matrix::operator*(const Matrix& other) const {
    assert(mNumCols == other.mNumRows); // Số cột ma trận trước = Số hàng ma trận sau
    Matrix temp(mNumRows, other.mNumCols);
    for (int i = 0; i < mNumRows; ++i) {
        for (int j = 0; j < other.mNumCols; ++j) {
            double sum = 0.0;
            for (int k = 0; k < mNumCols; ++k) {
                sum += mData[i][k] * other.mData[k][j];
            }
            temp.mData[i][j] = sum;
        }
    }
    return temp;
}

// Ma trận * Vector (Kết quả trả ra 1 Vector mới)
Vector Matrix::operator*(const Vector& vec) const {
    assert(mNumCols == vec.GetSize());
    Vector temp(mNumRows);
    for (int i = 0; i < mNumRows; ++i) {
        double sum = 0.0;
        for (int j = 0; j < mNumCols; ++j) {
            sum += mData[i][j] * vec[j]; // Dùng toán tử 0-based của Vector đã viết
        }
        temp[i] = sum;
    }
    return temp;
}

// Ma trận * Vô hướng (Scalar)
Matrix Matrix::operator*(double scalar) const {
    Matrix temp(mNumRows, mNumCols);
    for (int i = 0; i < mNumRows; ++i) {
        for (int j = 0; j < mNumCols; ++j) {
            temp.mData[i][j] = mData[i][j] * scalar;
        }
    }
    return temp;
}

Matrix operator*(double scalar, const Matrix& mat) {
    return mat * scalar;
}

// --- 7. ĐẠI SỐ TUYẾN TÍNH NÂNG CAO ---

// Hàm bổ trợ nội bộ: Loại bỏ hàng r và cột c để tạo ma trận con phục vụ tính Định thức
Matrix GetSubMatrix(const Matrix& mat, int r, int c) {
    Matrix sub(mat.GetNumberOfRows() - 1, mat.GetNumberOfCols() - 1);
    int subI = 1;
    for (int i = 1; i <= mat.GetNumberOfRows(); ++i) {
        if (i == r) continue;
        int subJ = 1;
        for (int j = 1; j <= mat.GetNumberOfCols(); ++j) {
            if (j == c) continue;
            sub(subI, subJ) = mat(i, j);
            subJ++;
        }
        subI++;
    }
    return sub;
}

// Tính định thức đệ quy Gauss/Laplace
double Matrix::Determinant() const {
    assert(mNumRows == mNumCols); // Bắt buộc phải là ma trận vuông
    
    if (mNumRows == 1) return mData[0][0];
    if (mNumRows == 2) return mData[0][0] * mData[1][1] - mData[0][1] * mData[1][0];

    double det = 0.0;
    int sign = 1;
    for (int j = 1; j <= mNumCols; ++j) {
        Matrix sub = GetSubMatrix(*this, 1, j);
        det += sign * mData[0][j - 1] * sub.Determinant();
        sign = -sign;
    }
    return det;
}

// Ma trận chuyển vị (Transpose)
Matrix Matrix::Transpose() const {
    Matrix temp(mNumCols, mNumRows);
    for (int i = 1; i <= mNumRows; ++i) {
        for (int j = 1; j <= mNumCols; ++j) {
            temp(j, i) = (*this)(i, j);
        }
    }
    return temp;
}

// Tính nghịch đảo bằng Ma trận Phụ hợp (Adjugate Matrix)
Matrix Matrix::Inverse() const {
    assert(mNumRows == mNumCols);
    double det = this->Determinant();
    if (std::abs(det) < 1e-9) {
        throw std::runtime_error("Ma tran suy bien (Det = 0), khong co ma tran nghich dao.");
    }

    Matrix adj(mNumRows, mNumCols);
    for (int i = 1; i <= mNumRows; ++i) {
        for (int j = 1; j <= mNumCols; ++j) {
            Matrix sub = GetSubMatrix(*this, i, j);
            double cofactor = sub.Determinant();
            if ((i + j) % 2 != 0) cofactor = -cofactor;
            adj(j, i) = cofactor; // Gán chuyển vị trực tiếp để tạo Ma trận phụ hợp
        }
    }
    return adj * (1.0 / det);
}

// Giả nghịch đảo Moore-Penrose: A^+ = (A^T * A)^(-1) * A^T
Matrix Matrix::PseudoInverse() const {
    Matrix AT = this->Transpose();
    Matrix ATA = AT * (*this);
    Matrix ATA_Inv = ATA.Inverse();
    return ATA_Inv * AT;
}

// --- OSTREAM OVERLOAD ---
std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
    for (int i = 0; i < mat.mNumRows; ++i) {
        os << "[ ";
        for (int j = 0; j < mat.mNumCols; ++j) {
            os << mat.mData[i][j] << " ";
        }
        os << "]\n";
    }
    return os;
}