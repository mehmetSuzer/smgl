
#ifndef __SPOT_LIGHT_H__
#define __SPOT_LIGHT_H__

#include "light.h"

class SpotLight : public Light {
private:
    const float a;
    const float b;
    const Vector3D position;
    const Vector3D direction;
    const float cosHalfFOVRadian;

public:
    SpotLight(const Vector3D& position_, const Color& color_, float a_, float b_, const Vector3D& direction_, float FOVRadian_);

    const Vector3D& getPosition(void) const;
    const Vector3D& getDirection(void) const;
    const LightInfo shine(const Vector3D& point) const override;
};

#endif // __SPOT_LIGHT_H__
