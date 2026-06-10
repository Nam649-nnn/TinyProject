#ifndef LINEARSYSTEM_HPP
#define LINEARSYSTEM_HPP

#include "Vector.hpp"
#include "Matrix.hpp"

// ==================== LỚP CHA: LINEAR SYSTEM ====================
class LinearSystem {
protected: // Đổi từ private thành protected theo yêu cầu để lớp con truy cập được
    int mSize;          // Kích thước hệ phương trình
    Matrix* mpA;        // Con trỏ trỏ tới ma trận hệ số A
    Vector* mpb;        // Con trỏ trỏ tới vector vế phải b

private:
    // Vô hiệu hóa copy constructor bằng cách để trong private (theo yêu cầu đề bài)
    LinearSystem(const LinearSystem& other);

public:
    // Chỉ cho phép khởi tạo thông qua constructor chuyên dụng (Không dùng default constructor)
    LinearSystem(const Matrix& A, const Vector& b);
    
    // Hàm hủy ảo (Virtual Destructor) cực kỳ quan trọng khi dùng kế thừa
    virtual ~LinearSystem();

    // Hàm giải hệ ảo để lớp con ghi đè
    virtual Vector Solve();
};

// ==================== LỚP CON: POSITIVE SYMMETRIC SYSTEM ====================
class PosSymLinSystem : public LinearSystem {
public:
    // Khởi tạo thông qua constructor của lớp cha
    PosSymLinSystem(const Matrix& A, const Vector& b);

    // Ghi đè phương thức giải bằng thuật toán Conjugate Gradient
    virtual Vector Solve() override;
};
// ==================== L?P CON NEW: NON-SQUARE SYSTEM (M?C 4) ====================
class NonSquareLinSystem : public LinearSystem {
private:
    double mAlpha; // H? s? di?u h�a Tikhonov (m?c d?nh b?ng 0 n?u ch? d�ng Moore-Penrose)

public:
    // Constructor nh?n v�o ma tr?n kh�ng vu�ng, vector b v� h? s? alpha t�y ch?n
    NonSquareLinSystem(const Matrix& A, const Vector& b, double alpha = 0.0);

    // Ghi d� phuong th?c Solve b?ng c�ng th?c gi? ngh?ch d?o / Tikhonov
    virtual Vector Solve() override;
};
#endif // LINEARSYSTEM_HPP
