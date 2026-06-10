#include "LinearSystem.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <iostream>

// ==================== IMPLEMENTATION: LINEAR SYSTEM (LỚP CHA) ====================

LinearSystem::LinearSystem(const Matrix& A, const Vector& b) {
    // Kiểm tra tính tương thích kích thước: A phải là ma trận vuông và hàng phải bằng cỡ của b
    assert(A.GetNumberOfRows() == A.GetNumberOfCols());
    assert(A.GetNumberOfRows() == b.GetSize());

    mSize = b.GetSize();
    
    // Cấp phát động bản sao độc lập cho ma trận và vector để tránh side-effect ngoài luồng
    mpA = new Matrix(A);
    mpb = new Vector(b);
}

LinearSystem::~LinearSystem() {
    delete mpA;
    delete mpb;
}

// Giải bằng thuật toán Khử Gauss có chọn phần tử trội (Gaussian Elimination with Pivoting)
Vector LinearSystem::Solve() {
    // Tạo bản sao cục bộ để không làm biến đổi ma trận gốc của hệ thống
    Matrix A = *mpA;
    Vector b = *mpb;
    Vector x(mSize);

    // 1. Quá trình khử xuôi (Forward Elimination) kèm Pivoting
    for (int k = 1; k <= mSize - 1; ++k) {
        
        // Chọn phần tử trội trên cột k (Partial Pivoting)
        double maxVal = std::abs(A(k, k));
        int pivotRow = k;
        for (int i = k + 1; i <= mSize; ++i) {
            if (std::abs(A(i, k)) > maxVal) {
                maxVal = std::abs(A(i, k));
                pivotRow = i;
            }
        }

        // Đổi chỗ hàng nếu tìm được phần tử trội tốt hơn
        if (pivotRow != k) {
            for (int j = 1; j <= mSize; ++j) {
                double tempA = A(k, j);
                A(k, j) = A(pivotRow, j);
                A(pivotRow, j) = tempA;
            }
            double tempb = b(k);
            b(k) = b(pivotRow);
            b(pivotRow) = tempb;
        }

        // Thực hiện khử các hàng phía dưới hàng k
        if (std::abs(A(k, k)) < 1e-9) {
            throw std::runtime_error("Ma tran suy bien hoặc gần suy biến, khong the dung Gauss.");
        }

        for (int i = k + 1; i <= mSize; ++i) {
            double factor = A(i, k) / A(k, k);
            for (int j = k; j <= mSize; ++j) {
                A(i, j) -= factor * A(k, j);
            }
            b(i) -= factor * b(k);
        }
    }

    // 2. Quá trình thế ngược (Back Substitution)
    if (std::abs(A(mSize, mSize)) < 1e-9) {
        throw std::runtime_error("Ma tran suy bien.");
    }
    x(mSize) = b(mSize) / A(mSize, mSize);

    for (int i = mSize - 1; i >= 1; --i) {
        double sum = 0.0;
        for (int j = i + 1; j <= mSize; ++j) {
            sum += A(i, j) * x(j);
        }
        x(i) = (b(i) - sum) / A(i, i);
    }

    return x;
}

// ==================== IMPLEMENTATION: POS_SYM_LIN_SYSTEM (LỚP CON) ====================

PosSymLinSystem::PosSymLinSystem(const Matrix& A, const Vector& b) : LinearSystem(A, b) {
    // Yêu cầu đề bài: Kiểm tra xem ma trận truyền vào có đối xứng (Symmetric) không
    for (int i = 1; i <= mSize; ++i) {
        for (int j = i + 1; j <= mSize; ++j) {
            if (std::abs((*mpA)(i, j) - (*mpA)(j, i)) > 1e-7) {
                throw std::runtime_error("Loi: Ma tran he so khong doi xung! Khong dung duoc PosSymLinSystem.");
            }
        }
    }
}

// Giải bằng phương pháp Độ dốc liên hợp (Conjugate Gradient Method)
Vector PosSymLinSystem::Solve() {
    Matrix& A = *mpA;
    Vector& b = *mpb;
    
    Vector x(mSize); // Điểm khởi tạo x_0 ban đầu là vector không rỗng [0, 0, ...]
    
    // r_0 = b - A * x_0. Vì x_0 = 0 nên r_0 = b
    Vector r = b - (A * x);
    Vector p = r; // Hướng tìm kiếm ban đầu p_0 = r_0
    
    double r_old_dot = r * r; // Tích vô hướng của r_k * r_k

    // Vòng lặp lặp tối đa mSize lần (Đặc tính lý thuyết của thuật toán CG)
    for (int k = 0; k < mSize; ++k) {
        if (r_old_dot < 1e-10) break; // Đã hội tụ về nghiệm chuẩn

        Vector Ap = A * p;
        double alpha = r_old_dot / (p * Ap); // Bước nhảy alpha

        x = x + (p * alpha);  // Cập nhật nghiệm mới
        r = r - (Ap * alpha); // Cập nhật phần dư mới

        double r_new_dot = r * r;
        if (r_new_dot < 1e-10) break;

        double beta = r_new_dot / r_old_dot;
        p = r + (p * beta); // Cập nhật hướng tìm kiếm liên hợp tiếp theo

        r_old_dot = r_new_dot;
    }

    return x;
}
// ==================== IMPLEMENTATION: NON_SQUARE_LIN_SYSTEM ====================

NonSquareLinSystem::NonSquareLinSystem(const Matrix& A, const Vector& b, double alpha) 
    : LinearSystem(A, b), mAlpha(alpha) {
    // Ghi d� ki?m tra c?a l?p cha: H? kh�ng vu�ng th� s? h�ng kh�ng c?n b?ng s? c?t n?a
    // Ch? c?n ki?m tra s? h�ng c?a A b?ng c? c?a b l� d? di?u ki?n k�ch thu?c
    assert(A.GetNumberOfRows() == b.GetSize());
}

Vector NonSquareLinSystem::Solve() {
    Matrix& A = *mpA;
    Vector& b = *mpb;

    // Tru?ng h?p 1: alpha == 0 -> D�ng thu?n Moore-Penrose Pseudo-Inverse: x = A^+ * b
    if (mAlpha <= 1e-11) {
        Matrix A_pseudo = A.PseudoInverse();
        return A_pseudo * b;
    } 
    
    // Tru?ng h?p 2: alpha > 0 -> �p d?ng di?u h�a Tikhonov (Tikhonov Regularization)
    // C�ng th?c t�nh: x = (A^T * A + alpha * I)^(-1) * A^T * b
    int cols = A.GetNumberOfCols();
    Matrix AT = A.Transpose();
    Matrix ATA = AT * A;

    // T?o ma tr?n don v? I c� c�ng k�ch c? v?i (A^T * A)
    Matrix I(cols, cols);
    for (int i = 1; i <= cols; ++i) {
        I(i, i) = 1.0;
    }

    // C?ng th�m th�nh ph?n di?u h�a d? l�m mu?t b�i to�n ill-posed: ATA + alpha * I
    Matrix TikhonovMatrix = ATA + (I * mAlpha);
    
    // Ngh?ch d?o v� t�nh to�n nghi?m cu?i c�ng
    Matrix TikhonovInv = TikhonovMatrix.Inverse();
    return TikhonovInv * (AT * b);
}
