
#ifndef __AABB_H__
#define __AABB_H__

#include <shape.h>

class AABB : public Shape {
private:
    Vector3D minPoint = Vector3D(-INFINITY);
    Vector3D maxPoint = Vector3D(INFINITY);

public:
    AABB();
    AABB(const Vector3D& minPoint_, const Vector3D& maxPoint_, const Color& color, float reflectivity, float transparency, float refractiveIndex);

    void setMinPoint(const Vector3D& point);
    void setMaxPoint(const Vector3D& point);

    bool intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const override;
    void findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const override;
};

#endif // __AABB_H__
