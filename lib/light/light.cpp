
#include "light.h"

Light::Light(const Color& color_) : color(color_) {}

const Color& Light::getColor(void) const {
    return color;
}
