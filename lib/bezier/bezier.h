
#ifndef __BEZIER_H__
#define __BEZIER_H__

#include <vector>
#include <triangle.h>
#include <aabb.h>

class BezierSurface : public Shape {
private:
    const Vector3D* controlPoints;
    const uint32_t subdivision;
    std::vector<Triangle> triangles;
    AABB boundingVolume;    

    void generateControlPointScalars(float* xVector, float x) const;
    Vector3D getPoint(float u, float v) const;

public:
    BezierSurface();
    BezierSurface(const Vector3D* controlPoints_, uint32_t subdivision_, const Color& color, float reflectivity, float transparency, float refractiveIndex);

    bool intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const override;
    void findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const override;
};

#endif // __BEZIER_H__
