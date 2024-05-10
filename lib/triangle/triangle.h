
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <matrix3x3.h>
#include <color.h>
#include <vector3d.h>
#include <shape.h>

#define DEFAULT_TRIANGLE_DIFFUSE_COEF   0.8f
#define DEFAULT_TRIANGLE_SPECULAR_COEF  0.6f
#define DEFAULT_TRIANGLE_SPECULAR_POW   8.0f
#define DEFAULT_TRIANGLE_AMBIENT_COEF   0.05f
#define DEFAULT_TRIANGLE_REFLECT        true

class Triangle : public Shape {
public:
    Vector3D a;
    Vector3D b;
    Vector3D c;
    Vector3D normal;

    Triangle();
    Triangle(const Vector3D& a, const Vector3D& b, const Vector3D& c, const Color& color,
        float diffuse_coef = DEFAULT_TRIANGLE_DIFFUSE_COEF, float specular_coef = DEFAULT_TRIANGLE_SPECULAR_COEF, 
        float specular_pow = DEFAULT_TRIANGLE_SPECULAR_POW, float ambient_coef = DEFAULT_TRIANGLE_AMBIENT_COEF, bool reflect = DEFAULT_TRIANGLE_REFLECT);
    
    bool intersect(Intersect *intersect, const Vector3D& source, const Vector3D& ray) const override;
};

#endif // __TRIANGLE_H__
