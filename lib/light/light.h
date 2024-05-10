
#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <color.h>
#include <vector3d.h>
#include <matrix3x3.h>

class Light {
public:
    Vector3D position;
    float intensity;
    Color color;

    Light();
    Light(const Vector3D& position, float intensity, const Color& color);
};

#endif // __LIGHT_H__
