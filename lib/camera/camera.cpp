
#include "camera.h"

Camera::Camera(uint32_t width, uint32_t height, Vector3D lower_left, Vector3D upper_right, Vector3D position) {
    Camera::width = width;
    Camera::height = height;
    Camera::position = position;

    Camera::lower_left = lower_left;
    Camera::upper_right = upper_right;
}
    
Vector3D Camera::generate_ray(float x, float y) {
    if (x < 0.0f || x > 1.0f || y < 0.0f || y > 1.0f) {
        return Vector3D();
    } else {
        return (lower_left + (upper_right-lower_left).mul_scale(x, y, 0.0f) - position).normalize();
    }
}