
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

Matrix3x3::Matrix3x3(const float* xs) {
    uint32_t index = 0;
    for (uint32_t i = 0; i < 3; i++) {
        Matrix3x3::xs[i][0] = xs[index];
        Matrix3x3::xs[i][1] = xs[index+1];
        Matrix3x3::xs[i][2] = xs[index+2];
        index += 3;
    }
}

Matrix3x3 Matrix3x3::operator + (const Matrix3x3& other) const {
    float resultXS[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        resultXS[i][0] = xs[i][0] + other.xs[i][0];
        resultXS[i][1] = xs[i][1] + other.xs[i][1];
        resultXS[i][2] = xs[i][2] + other.xs[i][2];
    }

    return Matrix3x3((float *)resultXS);
}

Matrix3x3 Matrix3x3::operator - (const Matrix3x3& other) const {
    float resultXS[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        resultXS[i][0] = xs[i][0] - other.xs[i][0];
        resultXS[i][1] = xs[i][1] - other.xs[i][1];
        resultXS[i][2] = xs[i][2] - other.xs[i][2];
    }

    return Matrix3x3((float *)resultXS);
}

Matrix3x3 Matrix3x3::operator * (float scalar) const {
    float resultXS[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        resultXS[i][0] = xs[i][0] * scalar;
        resultXS[i][1] = xs[i][1] * scalar;
        resultXS[i][2] = xs[i][2] * scalar;
    }

    return Matrix3x3((float *)resultXS);
}

Matrix3x3 Matrix3x3::operator * (const Matrix3x3& matrix) const {
    float resultXS[3][3]; 
    for (uint32_t i = 0; i < 3; i++) {
        for (uint32_t j = 0; j < 3; j++) {
            resultXS[i][j] = xs[i][0] * matrix.xs[0][j]
                            + xs[i][1] * matrix.xs[1][j]
                            + xs[i][2] * matrix.xs[2][j];
        }
    }

    return Matrix3x3((float *)resultXS);
}

Vector3D Matrix3x3::operator * (const Vector3D& vec) const {
    const float x = xs[0][0] * vec.x + xs[0][1] * vec.y + xs[0][2] * vec.z;
    const float y = xs[1][0] * vec.x + xs[1][1] * vec.y + xs[1][2] * vec.z;
    const float z = xs[2][0] * vec.x + xs[2][1] * vec.y + xs[2][2] * vec.z;

    return Vector3D(x, y, z);
}

Matrix3x3 Matrix3x3::transpose(void) const {
    float resultXS[3][3];
    for (uint32_t i = 0; i < 3; i++) {
        resultXS[i][0] = xs[0][i];
        resultXS[i][1] = xs[1][i];
        resultXS[i][2] = xs[2][i];
    }

    return Matrix3x3((float *)resultXS);
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
    return Vector3D(
        xs[0][column], 
        xs[1][column], 
        xs[2][column]
    );
}

Vector3D Matrix3x3::row(uint32_t row) const {
    return Vector3D(
        xs[row][0], 
        xs[row][1], 
        xs[row][2]
    );
}

void Matrix3x3::swapRows(uint32_t row1, uint32_t row2) {
    const float temp[3] = {
        xs[row1][0], 
        xs[row1][1], 
        xs[row1][2]
    };

    xs[row1][0] = xs[row2][0];
    xs[row1][1] = xs[row2][1];
    xs[row1][2] = xs[row2][2];

    xs[row2][0] = temp[0];
    xs[row2][1] = temp[1];
    xs[row2][2] = temp[2];
}

void Matrix3x3::swapColumns(uint32_t column1, uint32_t column2) {
    const float temp[3] = {
        xs[0][column1], 
        xs[1][column1], 
        xs[2][column1]
    };

    xs[0][column1] = xs[0][column2];
    xs[1][column1] = xs[1][column2];
    xs[2][column1] = xs[2][column2];

    xs[0][column2] = temp[0];
    xs[1][column2] = temp[1];
    xs[2][column2] = temp[2];
}

Vector3D Matrix3x3::cramersRule(const Vector3D& vector) const {
    const float determinant = det();
    const float x = Matrix3x3(vector, column(1), column(2)).det() / determinant;
    const float y = Matrix3x3(column(0), vector, column(2)).det() / determinant;
    const float z = Matrix3x3(column(0), column(1), vector).det() / determinant;

    return Vector3D(x, y, z);
}

Vector3D Matrix3x3::solve(Matrix3x3& matrix, Vector3D& vector) {
    // Swap rows so that matrix[0][0] is not zero
    if (abs(matrix.xs[0][0]) < EPSILON5) {
        const float temp = vector.x;
        if (abs(matrix.xs[1][0]) > EPSILON5) {
            matrix.swapRows(0, 1);
            vector.x = vector.y;
            vector.y = temp;
        } else {
            matrix.swapRows(0, 2);
            vector.x = vector.z;
            vector.z = temp;
        }
    }

    if (abs(matrix.xs[1][0]) > EPSILON5) {
        const float scalar0 = matrix.xs[1][0] / matrix.xs[0][0];
        matrix.xs[1][0] = 0.0f;
        matrix.xs[1][1] -= matrix.xs[0][1] * scalar0;
        matrix.xs[1][2] -= matrix.xs[0][2] * scalar0;
        vector.y -= vector.x * scalar0;
    }

    if (abs(matrix.xs[2][0]) > EPSILON5) {
        const float scalar1 = matrix.xs[2][0] / matrix.xs[0][0];
        matrix.xs[2][0] = 0.0f;
        matrix.xs[2][1] -= matrix.xs[0][1] * scalar1;
        matrix.xs[2][2] -= matrix.xs[0][2] * scalar1;
        vector.z -= vector.x * scalar1;
    }

    // Ensure that matrix[1][1] is not zero
    if (abs(matrix.xs[1][1]) > EPSILON5) {
        const float scalar2 = matrix.xs[2][1] / matrix.xs[1][1];
        matrix.xs[2][1] = 0.0f;
        matrix.xs[2][2] -= matrix.xs[1][2] * scalar2;
        vector.z -= vector.y * scalar2;
    } else {
        matrix.swapRows(1, 2);
        const float temp = vector.y;
        vector.y = vector.z;
        vector.z = temp;
    }

    const float z = vector.z / matrix.xs[2][2];
    const float y = (vector.y - matrix.xs[1][2] * z) / matrix.xs[1][1];
    const float x = (vector.x - matrix.xs[0][1] * y - matrix.xs[0][2] * z) / matrix.xs[0][0];

    return Vector3D(x, y, z);
}
