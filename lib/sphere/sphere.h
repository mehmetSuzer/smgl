
#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <vector3d.h>
#include <color.h>
#include <shape.h>

class Sphere : public Shape {
private:
    const Vector3D center;
    const float radius;

public:
    Sphere();
    Sphere(const Vector3D& center_, float radius_, const Color& color, bool reflect, float transparency, float refractive_index);

    bool intersect(Intersect* intersect, const Ray& ray) const override;
};

#endif // __SPHERE_H__
