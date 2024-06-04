
#ifndef __BEZIER_H__
#define __BEZIER_H__

#include <vector>
#include <triangle.h>
#include <aabb.h>

class BezierSurface : public Shape {
private:
    const uint32_t subdivision;
    std::vector<Triangle> triangles;
    AABB boundingVolume;    

    void generateControlPointScalars(float* x_vector, float x) const;
    Vector3D getPoint(const Vector3D* controls, float u, float v) const;
    void findBoundingVolume(const Vector3D* controls);

public:
    BezierSurface();
    BezierSurface(Vector3D* controls, uint32_t subdivision_, const Color& color, bool reflect, float transparency, float refractive_index);

    bool intersect(Intersect* intersect, const Ray& ray, float far) const override;
};

#endif // __BEZIER_H__
