
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <matrix3x3.h>
#include <color.h>
#include <vector3d.h>
#include <shape.h>

class Triangle : public Shape {
public:
    const Vector3D a;
    const Vector3D b;
    const Vector3D c;
    const Vector3D normal;

    Triangle();
    Triangle(const Vector3D& a_, const Vector3D& b_, const Vector3D& c_, const Color& color, bool reflect);
    
    bool intersect(Intersect *intersect, const Ray& ray) const override;
};

#endif // __TRIANGLE_H__
