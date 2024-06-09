
#include "shape.h"

Shape::Shape(const Color& color_, float reflectivity_, float transparency_, float refractiveIndex_) 
    : color(color_), reflectivity(reflectivity_), transparency(transparency_), refractiveIndex(refractiveIndex_) {
    
    assert(0.0f <= reflectivity_ && 0.0f <= transparency_);
    assert(reflectivity_ + transparency_ < 1.0f);
    assert(refractiveIndex_ >= VACUUM_REFRACTIVE_INDEX);
}

float Shape::getTransparency(void) const {
    return transparency;
}

float Shape::getReflectivity(void) const {
    return reflectivity;
}

float Shape::getRefractiveIndex(void) const {
    return refractiveIndex;
}

const Color& Shape::getColor(void) const {
    return color;
}
