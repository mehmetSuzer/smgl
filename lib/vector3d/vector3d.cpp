
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

Vector3D Vector3D::scale(float scalar_x, float scalar_y, float scalar_z) const {
    return Vector3D(x*scalar_x, y*scalar_y, z*scalar_z);
}

float Vector3D::dot(const Vector3D& other) const {
    return x*other.x + y*other.y + z*other.z;
}

float Vector3D::mag(void) const {
    return sqrtf(x*x + y*y + z*z);
}

float Vector3D::mag_square(void) const {
    return x*x + y*y + z*z;
}

void Vector3D::normalized(void) {
    float magnitude = mag();
    assert(magnitude != 0.0f);

    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

Vector3D Vector3D::normalize(void) const {
    float magnitude = mag();
    assert(magnitude != 0.0f);

    return Vector3D(x/magnitude ,y/magnitude ,z/magnitude);
}

Vector3D Vector3D::cross(const Vector3D& other) const {
    return Vector3D(y*other.z - z*other.y,
                    z*other.x - x*other.z,
                    x*other.y - y*other.x);
}

void Vector3D::rotate_x(float radian) {
    if (radian != 0.0f) {
        const float cos_angle = cosf(radian);
        const float sin_angle = sinf(radian);
        const float new_y = y*cos_angle - z*sin_angle;
        const float new_z = y*sin_angle + z*cos_angle;

        y = new_y;
        z = new_z;
    }
}

void Vector3D::rotate_y(float radian) {
    if (radian != 0.0f) {
        const float cos_angle = cosf(radian);
        const float sin_angle = sinf(radian);
        const float new_x = x*cos_angle - z*sin_angle;
        const float new_z = x*sin_angle + z*cos_angle;

        x = new_x;
        z = new_z;
    }
}

void Vector3D::rotate_z(float radian) {
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
    rotate_x(radian_x);
    rotate_y(radian_y);
    rotate_z(radian_z);
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
