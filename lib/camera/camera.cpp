
#include "camera.h"

Camera::Camera(const Vector3D& position_, const Vector3D& direction_, const Vector3D& up_, 
    float near_, float FOV_radian_, uint32_t width_, uint32_t height_) : position(position_) {
        const Vector3D right = up_.cross(direction_);

        screen_half_width = near_ * tanf(FOV_radian_ / 2.0f);
        screen_half_height = (screen_half_width / height_) * width_;
        lower_left = direction_*near_ - (right*screen_half_width) - (up_*screen_half_height);

        right_per_x = right * (2.0f * screen_half_width);
        up_per_y = up_ * (2.0f * screen_half_height);
    }

// Generates a ray for the given point on the screen 
// x and y must be in [0.0,1.0]
Ray Camera::generateRay(float x, float y) const {
    assert(0.0f <= x && x <= 1.0f && 0.0f <= y && y <= 1.0f);
    const Vector3D pixel_position_wrt_camera_position = lower_left + right_per_x*x + up_per_y*y;

    return Ray{
        .origin = position + pixel_position_wrt_camera_position, 
        .dir = pixel_position_wrt_camera_position.normalize(),
    };
}
