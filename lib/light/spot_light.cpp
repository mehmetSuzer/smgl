
#include "spot_light.h"

SpotLight::SpotLight(const Vector3D& position_, const Color& color_, float a_, float b_, const Vector3D& direction_, float FOVRadian_)
    : Light(color_), a(a_), b(b_), position(position_), direction(direction_), cosHalfFOVRadian(cosf(FOVRadian_ / 2.0f)) {
    assert(0.0f < FOVRadian_ && FOVRadian_ <= M_PIf);
    assert(abs(direction_.magSquare() - 1.0f) < EPSILON6);
}

const Vector3D& SpotLight::getPosition(void) const {
    return position;
}

const Vector3D& SpotLight::getDirection(void) const {
    return direction;
}

const LightInfo SpotLight::shine(const Vector3D& point) const {
    const Vector3D positionToPoint = point - position;
    const float distance = positionToPoint.mag();
    const Vector3D directionToPoint = positionToPoint / distance;

    if (directionToPoint.dot(direction) <= cosHalfFOVRadian) {
        return LightInfo{ .distance = -INFINITY };
    }

    const float intensity = 1.0f / ((a * distance + b) * distance + 1.0f);
    return LightInfo{
        -directionToPoint,
        distance,
        intensity,
    };
}
