#include "Vector.hpp"
#include <cassert>
#include <stdexcept>

// 1. Constructors & Destructor
Vector::Vector() : mSize(0), mData(nullptr) {}

Vector::Vector(int size) : mSize(size) {
    assert(size >= 0);
    if (size > 0) {
        mData = new double[mSize];
        for (int i = 0; i < mSize; ++i) {
            mData[i] = 0.0; // Initialize to zero
        }
    } else {
        mData = nullptr;
    }
}

Vector::Vector(const Vector& other) : mSize(other.mSize) {
    if (mSize > 0) {
        mData = new double[mSize];
        for (int i = 0; i < mSize; ++i) {
            mData[i] = other.mData[i];
        }
    } else {
        mData = nullptr;
    }
}

Vector::~Vector() {
    delete[] mData;
}

// Getter
int Vector::GetSize() const {
    return mSize;
}

// 2. Assignment Operator
Vector& Vector::operator=(const Vector& other) {
    if (this != &other) { // Check for self-assignment
        delete[] mData;   // Free existing memory

        mSize = other.mSize;
        if (mSize > 0) {
            mData = new double[mSize];
            for (int i = 0; i < mSize; ++i) {
                mData[i] = other.mData[i];
            }
        } else {
            mData = nullptr;
        }
    }
    return *this;
}

// 3. Unary Operators
Vector Vector::operator+() const {
    return *this; // Returns a copy of itself
}

Vector Vector::operator-() const {
    Vector temp(mSize);
    for (int i = 0; i < mSize; ++i) {
        temp.mData[i] = -mData[i];
    }
    return temp;
}

// 4. Binary Operators (Vector & Vector)
Vector Vector::operator+(const Vector& other) const {
    assert(mSize == other.mSize);
    Vector temp(mSize);
    for (int i = 0; i < mSize; ++i) {
        temp.mData[i] = mData[i] + other.mData[i];
    }
    return temp;
}

Vector Vector::operator-(const Vector& other) const {
    assert(mSize == other.mSize);
    Vector temp(mSize);
    for (int i = 0; i < mSize; ++i) {
        temp.mData[i] = mData[i] - other.mData[i];
    }
    return temp;
}

// Dot Product
double Vector::operator*(const Vector& other) const {
    assert(mSize == other.mSize);
    double dotProduct = 0.0;
    for (int i = 0; i < mSize; ++i) {
        dotProduct += mData[i] * other.mData[i];
    }
    return dotProduct;
}

// 5. Binary Operators (Vector & Scalar)
Vector Vector::operator*(double scalar) const {
    Vector temp(mSize);
    for (int i = 0; i < mSize; ++i) {
        temp.mData[i] = mData[i] * scalar;
    }
    return temp;
}

// Permute scalar * Vector order
Vector operator*(double scalar, const Vector& vec) {
    return vec * scalar; 
}

// 6. Indexing & Range Checking
// 0-based indexing [ ] with safety checks
double& Vector::operator[](int index) {
    if (index < 0 || index >= mSize) {
        throw std::out_of_range("Vector index out of bounds (0-based)");
    }
    return mData[index];
}

double Vector::operator[](int index) const {
    if (index < 0 || index >= mSize) {
        throw std::out_of_range("Vector index out of bounds (0-based)");
    }
    return mData[index];
}

// 1-based indexing ( )
double& Vector::operator()(int index) {
    if (index < 1 || index > mSize) {
        throw std::out_of_range("Vector index out of bounds (1-based)");
    }
    return mData[index - 1]; // Maps 1 to 0, 2 to 1, etc.
}

double Vector::operator()(int index) const {
    if (index < 1 || index > mSize) {
        throw std::out_of_range("Vector index out of bounds (1-based)");
    }
    return mData[index - 1];
}

// Ostream overload for debugging and output
std::ostream& operator<<(std::ostream& os, const Vector& vec) {
    os << "[ ";
    for (int i = 0; i < vec.mSize; ++i) {
        os << vec.mData[i] << " ";
    }
    os << "]";
    return os;
}