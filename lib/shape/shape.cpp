
#include "shape.h"

Shape::Shape(Shape_Type type, const Color& color, float diffuse_coef, float specular_coef, float specular_pow, float ambient_coef, bool reflect) {
    Shape::type = type;
    Shape::color = color;
    Shape::diffuse_coef = diffuse_coef;
    Shape::specular_coef = specular_coef;
    Shape::specular_pow = specular_pow;
    Shape::ambient_coef = ambient_coef;
    Shape::reflect = reflect;
}
