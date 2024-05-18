
#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

#include <assert.h>
#include <math.h>

class Vector3D {
public:
    float x, y, z;

    Vector3D();
    Vector3D(float x_, float y_, float z_);

    Vector3D operator + (const Vector3D& other) const;
    void operator += (const Vector3D& other);
    Vector3D operator - () const;
    Vector3D operator - (const Vector3D& other) const;
    void operator -= (const Vector3D& other);
    Vector3D operator * (float scalar) const;
    void operator *= (float scalar);
    Vector3D mul_scale(float scalar_x, float scalar_y, float scalar_z) const;

    float dot(const Vector3D& other) const;
    float mag(void) const;
    float mag_square(void) const;

    void normalized(void);
    Vector3D normalize(void) const;
    Vector3D cross(const Vector3D& other) const;

    static Vector3D projection(const Vector3D& vector, const Vector3D& onto);
    static Vector3D bisector(const Vector3D& vec1, const Vector3D& vec2);
    static Vector3D reflection(const Vector3D& unit, const Vector3D& normal);
};

typedef struct {
    Vector3D source;
    Vector3D dir;
} Ray;

#endif // __VECTOR3D_H__


