
#ifndef __AABB_H__
#define __AABB_H__

#include <iostream>
#include <shape.h>

class AABB : public Shape {
private:
    Vector3D minPoint;
    Vector3D maxPoint;

public:
    AABB();
    AABB(const Vector3D& minPoint_, const Vector3D& maxPoint_, const Color& color, bool reflect, float transparency, float refractive_index);

    void setMinPoint(const Vector3D& point);
    void setMaxPoint(const Vector3D& point);

    bool intersect(Intersect* intersect, const Ray& ray, float far) const override;
};

#endif // __AABB_H__
