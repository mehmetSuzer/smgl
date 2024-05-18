
#include "matrix3x3.h"

Matrix3x3::Matrix3x3() {}

Matrix3x3::Matrix3x3(const Vector3D& vector0, const Vector3D& vector1, const Vector3D& vector2, bool column) {
    if (column) {
        xs[0][0] = vector0.x;
        xs[1][0] = vector0.y;
        xs[2][0] = vector0.z;
        xs[0][1] = vector1.x;
        xs[1][1] = vector1.y;
        xs[2][1] = vector1.z;
        xs[0][2] = vector2.x;
        xs[1][2] = vector2.y;
        xs[2][2] = vector2.z;
    } else {
        xs[0][0] = vector0.x;
        xs[0][1] = vector0.y;
        xs[0][2] = vector0.z;
        xs[1][0] = vector1.x;
        xs[1][1] = vector1.y;
        xs[1][2] = vector1.z;
        xs[2][0] = vector2.x;
        xs[2][1] = vector2.y;
        xs[2][2] = vector2.z;
    }
}

Matrix3x3::Matrix3x3(float xs[3][3]) {
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            Matrix3x3::xs[i][j] = xs[i][j];
        }
    }
}

Matrix3x3 Matrix3x3::operator + (const Matrix3x3& other) const {
    float result_xs[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            result_xs[i][j] = xs[i][j] + other.xs[i][j];
        }
    }
    return Matrix3x3(result_xs);
}

Matrix3x3 Matrix3x3::operator - (const Matrix3x3& other) const {
    float result_xs[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            result_xs[i][j] = xs[i][j] - other.xs[i][j];
        }
    }
    return Matrix3x3(result_xs);
}

Matrix3x3 Matrix3x3::operator * (float scalar) const {
    float result_xs[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            result_xs[i][j] = xs[i][j] * scalar;
        }
    }
    return Matrix3x3(result_xs);
}

Matrix3x3 Matrix3x3::operator * (const Matrix3x3& matrix) const {
    float result_xs[3][3] = {0.0f}; 
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            for (uint32_t k = 0; k < 3; k++) {
                result_xs[i][j] = xs[i][k] + matrix.xs[k][j];
            }
        }
    }
    return Matrix3x3(result_xs);
}

Vector3D Matrix3x3::operator * (const Vector3D& vec) const {
    float result_x = xs[0][0] * vec.x + xs[0][1] * vec.y + xs[0][2] * vec.z;
    float result_y = xs[1][0] * vec.x + xs[1][1] * vec.y + xs[1][2] * vec.z;
    float result_z = xs[2][0] * vec.x + xs[2][1] * vec.y + xs[2][2] * vec.z;
    return Vector3D(result_x, result_y, result_z);
}

Matrix3x3 Matrix3x3::transpose(void) const {
    float result_xs[3][3];
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            result_xs[i][j] = xs[j][i];
        }
    }
    return Matrix3x3(result_xs);
}

float Matrix3x3::det(void) const {
    return + xs[0][0] * (xs[1][1] * xs[2][2] - xs[1][2] * xs[2][1])
           - xs[0][1] * (xs[1][0] * xs[2][2] - xs[1][2] * xs[2][0])
           + xs[0][2] * (xs[1][0] * xs[2][1] - xs[1][1] * xs[2][0]);
}

float Matrix3x3::trace(void) const {
    return xs[0][0] + xs[1][1] + xs[2][2];
}

Vector3D Matrix3x3::column(uint32_t column) const {
    return Vector3D(xs[0][column], xs[1][column], xs[2][column]);
}

Vector3D Matrix3x3::row(uint32_t row) const {
    return Vector3D(xs[row][0], xs[row][1], xs[row][2]);
}

Vector3D Matrix3x3::cramers_rule(const Vector3D& vector) const { // write this more efficiently
    float determinant = det();

    float result_x = Matrix3x3(vector, column(1), column(2)).det() / determinant;
    float result_y = Matrix3x3(column(0), vector, column(2)).det() / determinant;
    float result_z = Matrix3x3(column(0), column(1), vector).det() / determinant;

    return Vector3D(result_x, result_y, result_z);
}

Vector3D rotate_x(const Vector3D& vector, float radian) {
    if (radian == 0.0f) {
        return vector;
    } else {
        Matrix3x3 matrix = Matrix3x3({{cosf(radian), -sinf(radian), 0.0f}, {sinf(radian), cosf(radian), 0.0f}, {0.0f, 0.0f, 1.0f}});
        return matrix * vector;
    }
}

Vector3D rotate_y(const Vector3D& vector, float radian) {
    if (radian == 0.0f) {
        return vector;
    } else {
        Matrix3x3 matrix = Matrix3x3({{cosf(radian), 0.0f, sinf(radian)}, {0.0f, 1.0f, 0.0f}, {-sinf(radian), 0.0f, cosf(radian)}});
        return matrix * vector;
    }
}

Vector3D rotate_z(const Vector3D& vector, float radian) {
    if (radian == 0.0f) {
        return vector;
    } else {
        Matrix3x3 matrix = Matrix3x3({{1.0f, 0.0f, 0.0f}, {0.0f, cosf(radian), -sinf(radian)}, {0.0f, sinf(radian), cosf(radian)}});
        return matrix * vector;
    }
}

Vector3D rotate(const Vector3D& vector, float radian_x, float radian_y, float radian_z) {
    return rotate_z(rotate_y(rotate_x(vector, radian_x), radian_y), radian_z);
}

