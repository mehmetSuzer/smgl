
#include "vector3d.h"

Vector3D::Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3D::Vector3D(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}

Vector3D Vector3D::operator + (const Vector3D& other) const {
    return Vector3D(x+other.x, y+other.y, z+other.z);
}

Vector3D Vector3D::operator - () const {
    return Vector3D(-x, -y, -z);
}

Vector3D Vector3D::operator - (const Vector3D& other) const {
    return Vector3D(x-other.x, y-other.y, z-other.z);
}

Vector3D Vector3D::operator * (float scalar) const {
    return Vector3D(x*scalar, y*scalar, z*scalar);
}

Vector3D Vector3D::mul_scale(float scalar_x, float scalar_y, float scalar_z) const {
    return Vector3D(x*scalar_x, y*scalar_y, z*scalar_z);
}

float Vector3D::dot(const Vector3D& other) const {
    return x*other.x + y*other.y + z*other.z;
}

float Vector3D::mag(void) const {
    return sqrt(x*x + y*y + z*z);
}

float Vector3D::mag_square(void) const {
    return x*x + y*y + z*z;
}

Vector3D Vector3D::normalize(void) const {
    float magnitude = mag();
    if (magnitude != 0) {
        return Vector3D(x/magnitude ,y/magnitude ,z/magnitude);
    } else {
        return Vector3D();
    }
}

Vector3D Vector3D::cross(const Vector3D& other) const {
    return Vector3D(y*other.z - z*other.y,
                    z*other.x - x*other.z,
                    x*other.y - y*other.x);
}

Vector3D Vector3D::bisector(const Vector3D& unit1, const Vector3D& unit2) {
    return (unit1 + unit2).normalize();
}

Vector3D Vector3D::reflection(const Vector3D& unit, const Vector3D& unit_normal) {
    return unit_normal * 2.0f * unit_normal.dot(unit) - unit;
}
