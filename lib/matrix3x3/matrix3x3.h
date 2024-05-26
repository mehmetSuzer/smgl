
#ifndef __MATRIX3X3_H__
#define __MATRIX3X3_H__

#include <stdint.h>
#include <vector3d.h>

class Matrix3x3 {
public:
    float xs[3][3] = {0.0f};

    Matrix3x3();
    Matrix3x3(const Vector3D& vector0, const Vector3D& vector1, const Vector3D& vector2, bool column = true);
    Matrix3x3(const float* xs);

    Matrix3x3 operator + (const Matrix3x3& other) const;
    Matrix3x3 operator - (const Matrix3x3& other) const;
    Matrix3x3 operator * (float scalar) const;
    Matrix3x3 operator * (const Matrix3x3& matrix) const;
    Vector3D operator * (const Vector3D& vec) const;

    Matrix3x3 transpose(void) const;

    float det(void) const;
    float trace(void) const;

    Vector3D column(uint32_t column) const;
    Vector3D row(uint32_t row) const;

    void swapRows(uint32_t row1, uint32_t row2);
    void swapColumns(uint32_t column1, uint32_t column2);

    Vector3D cramers_rule(const Vector3D& vector) const;
    static Vector3D solve(Matrix3x3& matrix, Vector3D& vector);
};


#endif // __MATRIX3X3_H__
