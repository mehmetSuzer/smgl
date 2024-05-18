
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <assert.h>
#include <iostream>
#include <stdint.h>
#include <color.h>
#include <vector3d.h>

typedef struct {
    const Vector3D position;
    const Color color;
} Light;

class Camera {
private:
    const Vector3D position;
    const uint32_t width;
    const uint32_t height;
    
    // lower left corner and upper right corner must have the same z value
    const Vector3D lower_left;  // (0.0, 0.0)
    const Vector3D upper_right; // (1.0, 1.0)
    const Vector3D lower_left_minus_position;
    const Vector3D upper_right_minus_lower_left;

public:
    Camera(uint32_t width_, uint32_t height_, Vector3D lower_left_, Vector3D upper_right_, Vector3D position_);
    // x,y must be in [0.0,1.0]
    Ray generate_ray(float x, float y) const;
};

#endif // __CAMERA_H__
