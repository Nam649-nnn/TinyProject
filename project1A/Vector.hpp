#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

class Vector {
private:
    int mSize;         // Size of the array
    double* mData;     // Pointer to the dynamically allocated data element array

public:
    // Constructors & Destructor
    Vector();                          // Default constructor
    Vector(int size);                  // Parameterized constructor
    Vector(const Vector& other);       // Copy constructor
    ~Vector();                         // Destructor

    // Getters
    int GetSize() const;

    // Assignment Operator
    Vector& operator=(const Vector& other);

    // Unary Operators
    Vector operator+() const;          // Unary plus
    Vector operator-() const;          // Unary minus

    // Binary Operators (Vector-Vector)
    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    double operator*(const Vector& other) const; // Scalar/Dot product

    // Binary Operators (Vector-Scalar)
    Vector operator*(double scalar) const;

    // Indexing Operators
    // 0-based indexing with range checking
    double& operator[](int index);
    double operator[](int index) const;

    // 1-based indexing
    double& operator()(int index);
    double operator()(int index) const;

    // Friend function for scalar * Vector (e.g., 2.5 * vec)
    friend Vector operator*(double scalar, const Vector& vec);
    
    // Friend function for easy printing
    friend std::ostream& operator<<(std::ostream& os, const Vector& vec);
};

#endif // VECTOR_HPP