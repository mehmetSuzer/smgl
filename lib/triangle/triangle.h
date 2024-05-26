
#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include <shape.h>
#include <matrix3x3.h>

class Triangle : public Shape {
private:
    const Vector3D a;
    const Vector3D b;
    const Vector3D c;
    const Vector3D normal;

public:
    Triangle();
    Triangle(const Vector3D& a_, const Vector3D& b_, const Vector3D& c_, const Color& color, bool reflect, float transparency, float refractive_index);
    
    bool intersect(Intersect* intersect, const Ray& ray) const override;
};

#endif // __TRIANGLE_H__
