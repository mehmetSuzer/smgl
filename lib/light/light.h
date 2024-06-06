
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <color.h>
#include <vector3d.h>

typedef struct {
    Vector3D directionToLight;
    float distance;
    float intensity;
} LightInfo;

class Light {
private:
    const Color& color;

public:
    Light(const Color& color_);

    const Color& getColor(void) const;

    // Returns .distance = -INFINITY if the point cannot get light from the light source
    virtual const LightInfo shine(const Vector3D& point) const = 0;
};

#endif // __LIGHT_H__
