#include <iostream>
#include "Vector.hpp"
#include "Matrix.hpp"
#include "LinearSystem.hpp"

int main() {
    std::cout << "========================================================\n";
    std::cout << "TEST 1 & 2: Giai cac he phuong trinh vuong (Gauss & CG)\n";
    
    // (Gi? nguyên ph?n logic test 1 và test 2 cu c?a b?n ? dây n?u c?n)
    
    std::cout << "========================================================\n\n";

    // Ð? BÀI TEST 3 (M?C 4): H? KHÔNG VUÔNG & ÐI?U HÒA TIKHONOV
    std::cout << "TEST 3: He khong vuong (Over-determined) & Tikhonov Regularization\n";

    // Ma tr?n A c? 3 hàng 2 c?t (Th?a phuong trình)
    Matrix A_rect(3, 2);
    A_rect(1,1) = 1.0; A_rect(1,2) = 1.0;
    A_rect(2,1) = 1.0; A_rect(2,2) = 2.0;
    A_rect(3,1) = 1.0; A_rect(3,2) = 3.0;

    Vector b_rect(3);
    b_rect(1) = 2.1; b_rect(2) = 3.9; b_rect(3) = 6.2;

    std::cout << "Ma tran he so khong vuong A_rect (3x2):\n" << A_rect;
    std::cout << "Vector ve phai b_rect: " << b_rect << "\n";

    // Cách A: Gi?i b?ng Moore-Penrose thu?n túy (H? s? alpha = 0.0)
    NonSquareLinSystem sys_pseudo(A_rect, b_rect, 0.0);
    Vector x_pseudo = sys_pseudo.Solve();
    std::cout << "\n=> Nghiem x (Dung Moore-Penrose thuan): " << x_pseudo << "\n";

    // Cách B: Gi?i b?ng di?u hòa Tikhonov ch?ng nhi?u (Ví d? ch?n alpha = 0.01)
    double alpha = 0.01;
    NonSquareLinSystem sys_tikhonov(A_rect, b_rect, alpha);
    Vector x_tikhonov = sys_tikhonov.Solve();
    std::cout << "=> Nghiem x (Dung Tikhonov Regularization voi alpha = " << alpha << "): " << x_tikhonov << "\n";
    std::cout << "========================================================\n";

    return 0;
}
