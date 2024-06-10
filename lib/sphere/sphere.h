
#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <shape.h>

class Sphere : public Shape {
private:
    const Vector3D center;
    const float radius;

public:
    Sphere();
    Sphere(const Vector3D& center_, float radius_, const Color& color, float reflectivity, float transparency, float refractiveIndex);

    bool intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const override;
    void findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const override;
};

#endif // __SPHERE_H__
