
#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <vector3d.h>
#include <color.h>
#include <shape.h>

class Sphere : public Shape {
public:
    const Vector3D center;
    const float radius;

    Sphere();
    Sphere(const Vector3D& center_, float radius_, const Color& color, bool reflect);

    bool intersect(Intersect *intersect, const Ray& ray) const override;
};

#endif // __SPHERE_H__
