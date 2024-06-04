
#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

#include <assert.h>
#include <math.h>

#define smaller(x, y) ((x > y) ? y : x)
#define greater(x, y) ((x > y) ? x : y)

class Vector3D {
public:
    float x, y, z;

    static const Vector3D left;
    static const Vector3D right;
    static const Vector3D down;
    static const Vector3D up;
    static const Vector3D backward;
    static const Vector3D forward;

    Vector3D();
    Vector3D(float x_, float y_, float z_);
    
    Vector3D operator + (const Vector3D& other) const;
    void operator += (const Vector3D& other);
    Vector3D operator - (void) const;
    Vector3D operator - (const Vector3D& other) const;
    void operator -= (const Vector3D& other);
    Vector3D operator * (float scalar) const;
    void operator *= (float scalar);
    Vector3D operator / (float scalar) const;
    void operator /= (float scalar);

    Vector3D multiply(const Vector3D& multiplier) const;
    Vector3D divide(const Vector3D& divisor) const;

    float dot(const Vector3D& other) const;
    float mag(void) const;
    float magSquare(void) const;

    void normalized(void);
    Vector3D normalize(void) const;
    Vector3D cross(const Vector3D& other) const;

    void rotateX(float radian);
    void rotateY(float radian);
    void rotateZ(float radian);
    void rotate(float radianX, float radianY, float radianZ);

    static Vector3D projection(const Vector3D& vector, const Vector3D& onto);
    static Vector3D bisector(const Vector3D& unit1, const Vector3D& unit2);
    static Vector3D reflection(const Vector3D& unit, const Vector3D& unitNormal);
};

typedef struct {
    Vector3D origin;
    Vector3D dir;
} Ray;

#endif // __VECTOR3D_H__
