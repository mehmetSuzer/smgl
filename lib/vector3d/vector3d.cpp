
#include "vector3d.h"

Vector3D::Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3D::Vector3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

Vector3D Vector3D::operator + (const Vector3D& other) const {
    return Vector3D(x+other.x, y+other.y, z+other.z);
}

void Vector3D::operator += (const Vector3D& other) {
    x += other.x;
    y += other.y;
    z += other.z;
}

Vector3D Vector3D::operator - (void) const {
    return Vector3D(-x, -y, -z);
}

Vector3D Vector3D::operator - (const Vector3D& other) const {
    return Vector3D(x-other.x, y-other.y, z-other.z);
}

void Vector3D::operator -= (const Vector3D& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

Vector3D Vector3D::operator * (float scalar) const {
    return Vector3D(x*scalar, y*scalar, z*scalar);
}

void Vector3D::operator *= (float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

Vector3D Vector3D::operator / (float scalar) const {
    assert(scalar != 0.0f);
    return Vector3D(x/scalar, y/scalar, z/scalar);
}

void Vector3D::operator /= (float scalar) {
    assert(scalar != 0.0f);
    x /= scalar;
    y /= scalar;
    z /= scalar;
}

Vector3D Vector3D::multiply(const Vector3D& multiplier) const {
    return Vector3D(x*multiplier.x, y*multiplier.y, z*multiplier.z);
}

Vector3D Vector3D::divide(const Vector3D& divisor) const {
    assert(divisor.x != 0.0f);
    assert(divisor.y != 0.0f);
    assert(divisor.z != 0.0f);

    return Vector3D(x/divisor.x, y/divisor.y, z/divisor.z);
}

float Vector3D::dot(const Vector3D& other) const {
    return x*other.x + y*other.y + z*other.z;
}

float Vector3D::mag(void) const {
    return sqrtf(x*x + y*y + z*z);
}

float Vector3D::magSquare(void) const {
    return x*x + y*y + z*z;
}

void Vector3D::normalized(void) {
    float magnitude = mag();
    *this /= magnitude;
}

Vector3D Vector3D::normalize(void) const {
    float magnitude = mag();
    return *this / magnitude;
}

Vector3D Vector3D::cross(const Vector3D& other) const {
    return Vector3D(y*other.z - z*other.y,
                    z*other.x - x*other.z,
                    x*other.y - y*other.x);
}

void Vector3D::rotateX(float radian) {
    if (radian != 0.0f) {
        const float cos_angle = cosf(radian);
        const float sin_angle = sinf(radian);
        const float new_y = y*cos_angle - z*sin_angle;
        const float new_z = y*sin_angle + z*cos_angle;

        y = new_y;
        z = new_z;
    }
}

void Vector3D::rotateY(float radian) {
    if (radian != 0.0f) {
        const float cos_angle = cosf(radian);
        const float sin_angle = sinf(radian);
        const float new_x = x*cos_angle - z*sin_angle;
        const float new_z = x*sin_angle + z*cos_angle;

        x = new_x;
        z = new_z;
    }
}

void Vector3D::rotateZ(float radian) {
    if (radian != 0.0f) {
        const float cos_angle = cosf(radian);
        const float sin_angle = sinf(radian);
        const float new_x = x*cos_angle - y*sin_angle;
        const float new_y = x*sin_angle + y*cos_angle;

        x = new_x;
        y = new_y;
    }
}

void Vector3D::rotate(float radian_x, float radian_y, float radian_z) {
    rotateX(radian_x);
    rotateY(radian_y);
    rotateZ(radian_z);
}

Vector3D Vector3D::projection(const Vector3D& vector, const Vector3D& onto) {
    return onto * (onto.dot(vector) / onto.dot(onto));
}

Vector3D Vector3D::bisector(const Vector3D& unit1, const Vector3D& unit2) {
    return (unit1 + unit2).normalize();
}

Vector3D Vector3D::reflection(const Vector3D& unit, const Vector3D& unit_normal) {
    return unit_normal * 2.0f * unit_normal.dot(unit) - unit;
}

const Vector3D Vector3D::left = Vector3D(-1.0f, 0.0f, 0.0f);
const Vector3D Vector3D::right = Vector3D(1.0f, 0.0f, 0.0f);
const Vector3D Vector3D::down = Vector3D(0.0f, -1.0f, 0.0f);
const Vector3D Vector3D::up = Vector3D(0.0f, 1.0f, 0.0f);
const Vector3D Vector3D::backward = Vector3D(0.0f, 0.0f, -1.0f);
const Vector3D Vector3D::forward = Vector3D(0.0f, 0.0f, 1.0f);
