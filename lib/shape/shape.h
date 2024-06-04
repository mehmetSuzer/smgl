
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <color.h>
#include <vector3d.h>

#define DIFFUSE_COEF   0.8f
#define SPECULAR_COEF  0.5f
#define SPECULAR_POW   12.0f
#define AMBIENT_COEF   0.05f

#define VACUUM_REFRACTIVE_INDEX     1.00f
#define WATER_REFRACTIVE_INDEX      1.33f
#define ICE_REFRACTIVE_INDEX        1.31f
#define GLASS_REFRACTIVE_INDEX      1.52f
#define SAPPHIRE_REFRACTIVE_INDEX   1.77f
#define DIAMOND_REFRACTIVE_INDEX    2.417f

// Refractive index of the rendered world
#define WORLD_REFRACTIVE_INDEX VACUUM_REFRACTIVE_INDEX

// Use epsilons to avoid floating point rounding errors
#define EPSILON1 0.1f
#define EPSILON2 0.01f
#define EPSILON3 0.001f
#define EPSILON4 0.0001f
#define EPSILON5 0.00001f

typedef struct {
    float t;
    Vector3D hitLocation;
    Vector3D normal;
} Intersect;

class Shape {
private:
    const Color& color;
    const bool reflect;
    const float transparency;
    const float refractiveIndex;

public:
    Shape(const Color& color_, bool reflect_, float transparency_, float refractiveIndex_) 
        : color(color_), reflect(reflect_), transparency(transparency_), refractiveIndex(refractiveIndex_) {
        assert(0.0f <= transparency_ && transparency_ < 1.0f);
        assert(refractiveIndex_ >= VACUUM_REFRACTIVE_INDEX);
    }

    float getTransparency(void) const {
        return transparency;
    }

    bool isReflective(void) const {
        return reflect;
    }

    float getRefractiveIndex(void) const {
        return refractiveIndex;
    }

    const Color& getColor(void) const {
        return color;
    }

    virtual bool intersect(Intersect* intersect, const Ray& ray, float far) const = 0;
};

#endif // __SHAPE_H__
