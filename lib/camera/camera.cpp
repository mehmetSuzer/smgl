
#include "camera.h"

Camera::Camera(uint32_t width_, uint32_t height_, Vector3D lower_left_, Vector3D upper_right_, Vector3D position_) :
    width(width_), height(height_), lower_left(lower_left_), upper_right(upper_right_), position(position_),
    lower_left_minus_position(lower_left_-position_), upper_right_minus_lower_left(upper_right_-lower_left_) {}
    
Ray Camera::generate_ray(float x, float y) const {
    assert(0.0f <= x && x <= 1.0f && 0.0f <= y && y <= 1.0f);
    Vector3D dir = (lower_left_minus_position + upper_right_minus_lower_left.mul_scale(x, y, 0.0f)).normalize();
    return Ray{position, dir};
}
