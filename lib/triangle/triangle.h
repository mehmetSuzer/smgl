
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <shape.h>
#include <matrix3x3.h>

class Triangle : public Shape {
private:
    Vector3D points[3];
    Vector3D normal;

public:
    Triangle();
    Triangle(const Vector3D& a, const Vector3D& b, const Vector3D& c, const Color& color, float reflectivity, float transparency, float refractiveIndex);
    
    bool intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const override;
    void findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const override;
};

#endif // __TRIANGLE_H__
