
#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <vector3d.h>
#include <color.h>
#include <shape.h>

#define DEFAULT_SPHERE_DIFFUSE_COEF     0.8f
#define DEFAULT_SPHERE_SPECULAR_COEF    0.6f
#define DEFAULT_SPHERE_SPECULAR_POW     8.0f
#define DEFAULT_SPHERE_AMBIENT_COEF     0.05f
#define DEFAULT_SPHERE_REFLECT          false

class Sphere : public Shape {
public:
    Vector3D center;
    float radius;

    Sphere();
    Sphere(const Vector3D& center, float radius, const Color& color, float diffuse_coef = DEFAULT_SPHERE_DIFFUSE_COEF, 
    float specular_coef = DEFAULT_SPHERE_SPECULAR_COEF, float specular_pow = DEFAULT_SPHERE_SPECULAR_POW, 
    float ambient_coef = DEFAULT_SPHERE_AMBIENT_COEF, bool reflect = DEFAULT_SPHERE_REFLECT);

    bool intersect(Intersect *intersect, const Vector3D& source, const Vector3D& ray) const override;
};

#endif // __SPHERE_H__
