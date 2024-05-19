
#ifndef __BEZIER_H__
#define __BEZIER_H__

#include <assert.h>
#include <vector>
#include <triangle.h>

class BezierSurface : public Shape {
private:
    static const uint32_t subdivision = 4;
    std::vector<Vector3D> vertices;
    std::vector<Triangle> triangles;

    void generateControlPointScalars(float* x_vector, float x) const;
    Vector3D getPoint(Vector3D* controls, float u, float v) const;

public:
    BezierSurface();
    BezierSurface(Vector3D* controls, const Color& color, bool reflect, float transparency, float refractive_index);

    bool intersect(Intersect *intersect, const Ray& ray) const override;
};

#endif // __BEZIER_H__
