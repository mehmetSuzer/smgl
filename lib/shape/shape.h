
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <assert.h>
#include <color.h>
#include <vector3d.h>

#define DIFFUSE_COEF   0.8f
#define SPECULAR_COEF  0.6f
#define SPECULAR_POW   8.0f
#define AMBIENT_COEF   0.05f

#define VACUUM_REFRACTIVE_INDEX     1.00f
#define WATER_REFRACTIVE_INDEX      1.33f
#define ICE_REFRACTIVE_INDEX        1.31f
#define GLASS_REFRACTIVE_INDEX      1.52f
#define SAPPHIRE_REFRACTIVE_INDEX   1.77f
#define DIAMOND_REFRACTIVE_INDEX    2.417f

#define DEFAULT_REFRACTIVE_INDEX VACUUM_REFRACTIVE_INDEX

typedef struct {
    float t;
    Vector3D hit_location;
    Vector3D normal;
} Intersect;

class Shape {
public: // Make these protected and private
    const Color color;
    bool reflect;
    float transparency;
    float refractive_index;

    Shape(const Color& color_, bool reflect_, float transparency_, float refractive_index_) 
        : color(color_), reflect(reflect_), transparency(transparency_), refractive_index(refractive_index_) {
        assert(0.0f <= transparency_ && transparency_ < 1.0f);
        assert(refractive_index_ >= 1.0f);
    }

    virtual bool intersect(Intersect *intersect, const Ray& ray) const = 0;
};

#endif // __SHAPE_H__
