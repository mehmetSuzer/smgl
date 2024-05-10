
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <color.h>
#include <vector3d.h>

typedef struct {
    float t;
    Vector3D hit_location;
    Vector3D normal;
} Intersect;

typedef enum {NONE, SPHERE, TRIANGLE} Shape_Type;

class Shape {
public:
    Shape_Type type;
    Color color;
    float diffuse_coef;
    float specular_coef;
    float specular_pow;
    float ambient_coef;
    bool reflect;

    Shape(Shape_Type type, const Color& color, float diffuse_coef, float specular_coef, float specular_pow, float ambient_coef, bool reflect);

    virtual bool intersect(Intersect *intersect, const Vector3D& source, const Vector3D& ray) const = 0;
};

#endif // __SHAPE_H__
