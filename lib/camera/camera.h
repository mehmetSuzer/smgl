
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>
#include <vector3d.h>

class Camera {
public:
    Vector3D position;
    uint32_t width;
    uint32_t height;

private:
    // lower left corner and upper right corner must have the same z value
    Vector3D lower_left;  // (0.0, 0.0)
    Vector3D upper_right; // (1.0, 1.0)
    Vector3D d_pixel;

public:
    Camera(uint32_t width, uint32_t height, Vector3D lower_left, Vector3D upper_right, Vector3D position);
    // x,y must be in [0.0,1.0]
    Vector3D generate_ray(float x, float y);
};

#endif // __CAMERA_H__
