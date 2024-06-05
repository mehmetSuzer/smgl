
#include "camera.h"

Camera::Camera(const Vector3D& position_, const Vector3D& direction_, const Vector3D& up_, float near_, float far_, 
    float FOVRadian_, uint32_t width_, uint32_t height_) : position(position_), near(near_), far(far_) {
        
    assert(near_ > EPSILON3);
    assert(far_ > near_);
    assert(EPSILON1 < FOVRadian_ && FOVRadian_ < M_PIf * 0.98f);

    const Vector3D right = up_.cross(direction_);
    screenHalfWidth = near_ * tanf(FOVRadian_ / 2.0f);
    screenHalfHeight = (screenHalfWidth / height_) * width_;
    lowerLeft = direction_*near_ - right*screenHalfWidth - up_*screenHalfHeight;

    rightPerX = right * (2.0f * screenHalfWidth);
    upPerY = up_ * (2.0f * screenHalfHeight);
}

const Vector3D& Camera::getPosition(void) const {
    return position;
}

float Camera::getNear(void) const {
    return near;
}
    
float Camera::getFar(void) const {
    return far;
}

// Generates a ray for the given point on the screen 
// x and y must be in [0.0,1.0]
Ray Camera::generateRay(float x, float y) const {
    assert(0.0f <= x && x <= 1.0f && 0.0f <= y && y <= 1.0f);
    const Vector3D pixelPositionWRTCameraPosition = lowerLeft + rightPerX*x + upPerY*y;

    return Ray{
        .origin = position + pixelPositionWRTCameraPosition, 
        .dir = pixelPositionWRTCameraPosition.normalize(),
    };
}
