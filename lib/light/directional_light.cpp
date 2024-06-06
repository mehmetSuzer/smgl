
#include "directional_light.h"

DirectionalLight::DirectionalLight(const Vector3D& direction_, const Color& color_, float intensity_) 
    : Light(color_), direction(direction_), intensity(intensity_) {
    assert(0.0f < intensity_ && intensity_ <= 1.0f);
    assert(abs(direction_.mag() - 1.0f) < EPSILON6);
}

const Vector3D& DirectionalLight::getDirection(void) const {
    return direction;
}
    
const LightInfo DirectionalLight::shine(const Vector3D& point) const {
    return LightInfo{
        -direction,
        INFINITY,
        intensity,
    };
}
