
#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

#include "light.h"

class PointLight : public Light {
private:
    const float a;
    const float b;
    const Vector3D position;

public:
    PointLight(const Vector3D& position_, const Color& color_, float a_, float b_);

    const Vector3D& getPosition(void) const;
    const LightInfo shine(const Vector3D& point) const override;
};

#endif // __POINT_LIGHT_H__
