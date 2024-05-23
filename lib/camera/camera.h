
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <assert.h>
#include <stdint.h>
#include <color.h>
#include <vector3d.h>

typedef struct {
    const Vector3D& position;
    const Color& color;
} Light;

class Camera {
private:
    const Vector3D position;
    Vector3D lower_left;
    Vector3D right_per_x;
    Vector3D up_per_y;
    float screen_half_width;
    float screen_half_height;

public:
    Camera(const Vector3D& position_, const Vector3D& direction_, const Vector3D& up_, 
        float near_, float FOV_radian_, uint32_t width_, uint32_t height_);
        
    // x,y must be in [0.0,1.0]
    Ray generate_ray(float x, float y) const;
};

#endif // __CAMERA_H__
