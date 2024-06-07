
#ifndef __AABB_H__
#define __AABB_H__

#include <shape.h>

class AABB : public Shape {
private:
    Vector3D minPoint = Vector3D(-INFINITY, -INFINITY, -INFINITY);
    Vector3D maxPoint = Vector3D(INFINITY, INFINITY, INFINITY);

public:
    AABB();
    AABB(const Vector3D& minPoint_, const Vector3D& maxPoint_, const Color& color, float reflectivity, float transparency, float refractiveIndex);

    void setMinPoint(const Vector3D& point);
    void setMaxPoint(const Vector3D& point);

    bool intersect(Intersect* intersect, const Ray& ray, float far) const override;
};

#endif // __AABB_H__
