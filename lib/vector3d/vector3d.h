
#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

#include <math.h>

class Vector3D {
public:
    float x, y, z;

    Vector3D();
    Vector3D(float x_val, float y_val, float z_val);

    Vector3D operator + (const Vector3D& other) const;

    Vector3D operator - () const;

    Vector3D operator - (const Vector3D& other) const;

    Vector3D operator * (float scalar) const;

    Vector3D mul_scale(float scalar_x, float scalar_y, float scalar_z) const;

    float dot(const Vector3D& other) const;

    float mag(void) const;

    float mag_square(void) const;

    Vector3D normalize(void) const;

    Vector3D cross(const Vector3D& other) const;

    static Vector3D bisector(const Vector3D& vec1, const Vector3D& vec2);

    static Vector3D reflection(const Vector3D& unit, const Vector3D& normal);
};

#endif // __VECTOR3D_H__


