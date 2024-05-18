
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <color.h>
#include <vector3d.h>

#define DIFFUSE_COEF   0.8f
#define SPECULAR_COEF  0.6f
#define SPECULAR_POW   8.0f
#define AMBIENT_COEF   0.05f

typedef struct {
    float t;
    Vector3D hit_location;
    Vector3D normal;
} Intersect;

class Shape {
public:
    const Color color;
    const bool reflect;

    Shape(const Color& color_, bool reflect_) : color(color_), reflect(reflect_) {}
    virtual bool intersect(Intersect *intersect, const Ray& ray) const = 0;
};

#endif // __SHAPE_H__
