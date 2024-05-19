
#ifndef __MATRIX3X3_H__
#define __MATRIX3X3_H__

#include <stdint.h>
#include <vector3d.h>

class Matrix3x3 {
public:
    float xs[3][3] = {0.0f};

    Matrix3x3();
    Matrix3x3(const Vector3D& vector0, const Vector3D& vector1, const Vector3D& vector2, bool column = true);
    Matrix3x3(float xs[3][3]);

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
    Vector3D cramers_rule(const Vector3D& vector) const;

    static Vector3D rotate_x(const Vector3D& vector, float radian);
    static Vector3D rotate_y(const Vector3D& vector, float radian);
    static Vector3D rotate_z(const Vector3D& vector, float radian);

    static Vector3D rotate(const Vector3D& vector, float radian_x, float radian_y, float radian_z);
};


#endif // __MATRIX3X3_H__
