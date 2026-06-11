#include "LinearSystem.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <iostream>

// ==================== IMPLEMENTATION: LINEAR SYSTEM (Lá»šP CHA) ====================

LinearSystem::LinearSystem(const Matrix& A, const Vector& b) {
   
    assert(A.GetNumberOfRows() == b.GetSize());

    mSize = b.GetSize();
    
    // Allocate independent deep copies
    mpA = new Matrix(A);
    mpb = new Vector(b);
}

LinearSystem::~LinearSystem() {
    delete mpA;
    delete mpb;
}

// Giáº£i báº±ng thuáº­t toÃ¡n Khá»­ Gauss cÃ³ chá»n pháº§n tá»­ trá»™i (Gaussian Elimination with Pivoting)
Vector LinearSystem::Solve() {
    // Táº¡o báº£n sao cá»¥c bá»™ Ä‘á»ƒ khÃ´ng lÃ m biáº¿n Ä‘á»•i ma tráº­n gá»‘c cá»§a há»‡ thá»‘ng
    Matrix A = *mpA;
    Vector b = *mpb;
    Vector x(mSize);

    // 1. QuÃ¡ trÃ¬nh khá»­ xuÃ´i (Forward Elimination) kÃ¨m Pivoting
    for (int k = 1; k <= mSize - 1; ++k) {
        
        // Chá»n pháº§n tá»­ trá»™i trÃªn cá»™t k (Partial Pivoting)
        double maxVal = std::abs(A(k, k));
        int pivotRow = k;
        for (int i = k + 1; i <= mSize; ++i) {
            if (std::abs(A(i, k)) > maxVal) {
                maxVal = std::abs(A(i, k));
                pivotRow = i;
            }
        }

        // Äá»•i chá»— hÃ ng náº¿u tÃ¬m Ä‘Æ°á»£c pháº§n tá»­ trá»™i tá»‘t hÆ¡n
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

        // Thá»±c hiá»‡n khá»­ cÃ¡c hÃ ng phÃ­a dÆ°á»›i hÃ ng k
        if (std::abs(A(k, k)) < 1e-9) {
            throw std::runtime_error("Ma tran suy bien hoáº·c gáº§n suy biáº¿n, khong the dung Gauss.");
        }

        for (int i = k + 1; i <= mSize; ++i) {
            double factor = A(i, k) / A(k, k);
            for (int j = k; j <= mSize; ++j) {
                A(i, j) -= factor * A(k, j);
            }
            b(i) -= factor * b(k);
        }
    }

    // 2. QuÃ¡ trÃ¬nh tháº¿ ngÆ°á»£c (Back Substitution)
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

// ==================== IMPLEMENTATION: POS_SYM_LIN_SYSTEM (Lá»šP CON) ====================

PosSymLinSystem::PosSymLinSystem(const Matrix& A, const Vector& b) : LinearSystem(A, b) {
    // YÃªu cáº§u Ä‘á» bÃ i: Kiá»ƒm tra xem ma tráº­n truyá»n vÃ o cÃ³ Ä‘á»‘i xá»©ng (Symmetric) khÃ´ng
    for (int i = 1; i <= mSize; ++i) {
        for (int j = i + 1; j <= mSize; ++j) {
            if (std::abs((*mpA)(i, j) - (*mpA)(j, i)) > 1e-7) {
                throw std::runtime_error("Loi: Ma tran he so khong doi xung! Khong dung duoc PosSymLinSystem.");
            }
        }
    }
}

// Giáº£i báº±ng phÆ°Æ¡ng phÃ¡p Äá»™ dá»‘c liÃªn há»£p (Conjugate Gradient Method)
Vector PosSymLinSystem::Solve() {
    Matrix& A = *mpA;
    Vector& b = *mpb;
    
    Vector x(mSize); // Äiá»ƒm khá»Ÿi táº¡o x_0 ban Ä‘áº§u lÃ  vector khÃ´ng rá»—ng [0, 0, ...]
    
    // r_0 = b - A * x_0. VÃ¬ x_0 = 0 nÃªn r_0 = b
    Vector r = b - (A * x);
    Vector p = r; // HÆ°á»›ng tÃ¬m kiáº¿m ban Ä‘áº§u p_0 = r_0
    
    double r_old_dot = r * r; // TÃ­ch vÃ´ hÆ°á»›ng cá»§a r_k * r_k

    // VÃ²ng láº·p láº·p tá»‘i Ä‘a mSize láº§n (Äáº·c tÃ­nh lÃ½ thuyáº¿t cá»§a thuáº­t toÃ¡n CG)
    for (int k = 0; k < mSize; ++k) {
        if (r_old_dot < 1e-10) break; // ÄÃ£ há»™i tá»¥ vá» nghiá»‡m chuáº©n

        Vector Ap = A * p;
        double alpha = r_old_dot / (p * Ap); // BÆ°á»›c nháº£y alpha

        x = x + (p * alpha);  // Cáº­p nháº­t nghiá»‡m má»›i
        r = r - (Ap * alpha); // Cáº­p nháº­t pháº§n dÆ° má»›i

        double r_new_dot = r * r;
        if (r_new_dot < 1e-10) break;

        double beta = r_new_dot / r_old_dot;
        p = r + (p * beta); // Cáº­p nháº­t hÆ°á»›ng tÃ¬m kiáº¿m liÃªn há»£p tiáº¿p theo

        r_old_dot = r_new_dot;
    }

    return x;
}
// ==================== IMPLEMENTATION: NON_SQUARE_LIN_SYSTEM ====================

NonSquareLinSystem::NonSquareLinSystem(const Matrix& A, const Vector& b, double alpha) 
    : LinearSystem(A, b), mAlpha(alpha) {
    // Ghi dè ki?m tra c?a l?p cha: H? không vuông thì s? hàng không c?n b?ng s? c?t n?a
    // Ch? c?n ki?m tra s? hàng c?a A b?ng c? c?a b là d? di?u ki?n kích thu?c
    assert(A.GetNumberOfRows() == b.GetSize());
}

Vector NonSquareLinSystem::Solve() {
    Matrix& A = *mpA;
    Vector& b = *mpb;

    // Tru?ng h?p 1: alpha == 0 -> Dùng thu?n Moore-Penrose Pseudo-Inverse: x = A^+ * b
    if (mAlpha <= 1e-11) {
        Matrix A_pseudo = A.PseudoInverse();
        return A_pseudo * b;
    } 
    
    // Tru?ng h?p 2: alpha > 0 -> Áp d?ng di?u hòa Tikhonov (Tikhonov Regularization)
    // Công th?c tính: x = (A^T * A + alpha * I)^(-1) * A^T * b
    int cols = A.GetNumberOfCols();
    Matrix AT = A.Transpose();
    Matrix ATA = AT * A;

    // T?o ma tr?n don v? I có cùng kích c? v?i (A^T * A)
    Matrix I(cols, cols);
    for (int i = 1; i <= cols; ++i) {
        I(i, i) = 1.0;
    }

    // C?ng thêm thành ph?n di?u hòa d? làm mu?t bài toán ill-posed: ATA + alpha * I
    Matrix TikhonovMatrix = ATA + (I * mAlpha);
    
    // Ngh?ch d?o và tính toán nghi?m cu?i cùng
    Matrix TikhonovInv = TikhonovMatrix.Inverse();
    return TikhonovInv * (AT * b);
}
