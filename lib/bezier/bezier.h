
#ifndef __BEZIER_H__
#define __BEZIER_H__

#include <vector>
#include <triangle.h>

class BezierSurface : public Shape {
private:
    const uint32_t subdivision;
    std::vector<Triangle> triangles;
    
    Vector3D minPoint = Vector3D(INFINITY, INFINITY, INFINITY);
    Vector3D maxPoint = Vector3D(-INFINITY, -INFINITY, -INFINITY);

    void generateControlPointScalars(float* x_vector, float x) const;
    Vector3D getPoint(const Vector3D* controls, float u, float v) const;
    void findAABB(const Vector3D* controls);
    bool intersectAABB(const Ray& ray) const;

public:
    BezierSurface();
    BezierSurface(Vector3D* controls, uint32_t subdivision_, const Color& color, bool reflect, float transparency, float refractive_index);

    bool intersect(Intersect* intersect, const Ray& ray) const override;
};

#endif // __BEZIER_H__
