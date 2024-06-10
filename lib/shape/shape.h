
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <color.h>
#include <vector3d.h>

#define DIFFUSE_COEF   0.9f
#define SPECULAR_COEF  0.5f
#define SPECULAR_POW   12.0f
#define AMBIENT_COEF   0.1f

#define VACUUM_REFRACTIVE_INDEX     1.00000f
#define AIR_REFRACTIVE_INDEX        1.00026f
#define ICE_REFRACTIVE_INDEX        1.31000f
#define WATER_REFRACTIVE_INDEX      1.33000f
#define OIL_REFRACTIVE_INDEX        1.47000f
#define GLASS_REFRACTIVE_INDEX      1.52000f
#define SAPPHIRE_REFRACTIVE_INDEX   1.77000f
#define DIAMOND_REFRACTIVE_INDEX    2.41700f

// Refractive index of the rendered world
#define WORLD_REFRACTIVE_INDEX VACUUM_REFRACTIVE_INDEX

// Transparency of the rendered world
#define WORLD_TRANSPARENCY 1.0f

typedef struct {
    float t;
    Vector3D hitLocation;
    Vector3D normal;
} Intersect;

class Shape {
private:
    const Color& color;
    const float reflectivity;
    const float transparency;
    const float refractiveIndex;

public:
    Shape(const Color& color_, float reflectivity_, float transparency_, float refractiveIndex_);

    float getTransparency(void) const;
    float getReflectivity(void) const;
    float getRefractiveIndex(void) const;
    const Color& getColor(void) const;

    virtual bool intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const = 0;
    virtual void findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const = 0;
};

#endif // __SHAPE_H__
