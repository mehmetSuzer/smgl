
#ifndef __DIRECTIONAL_LIGHT_H__
#define __DIRECTIONAL_LIGHT_H__

#include "light.h"

class DirectionalLight : public Light {
private:    
    const float intensity;
    const Vector3D direction;

public:
    DirectionalLight(const Vector3D& direction_, const Color& color_, float intensity_);

    const Vector3D& getDirection(void) const;
    const LightInfo shine(const Vector3D& point) const override;
};

#endif // __DIRECTIONAL_LIGHT_H__
