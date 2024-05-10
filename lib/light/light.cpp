
#include "light.h"

Light::Light() {
    position = Vector3D();
    intensity = 1.0f;
    color = Color("BLACK");
}
    
Light::Light(const Vector3D& position, float intensity, const Color& color) {
    Light::position = position;
    Light::intensity = intensity;
    Light::color = color;
}
