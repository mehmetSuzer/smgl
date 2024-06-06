
#include "point_light.h"

PointLight::PointLight(const Vector3D& position_, const Color& color_, float a_, float b_) 
    : Light(color_), position(position_), a(a_), b(b_) {
    assert(a_ > 0.0f && b_ > 0.0f);
}

const Vector3D& PointLight::getPosition(void) const {
    return position;
}

const LightInfo PointLight::shine(const Vector3D& point) const {
    const Vector3D pointToPosition = position - point;
    const float distance = pointToPosition.mag();
    const float intensity = 1.0f / ((a * distance + b) * distance + 1.0f);

    return LightInfo{
        pointToPosition / distance,
        distance,
        intensity,
    };
}
