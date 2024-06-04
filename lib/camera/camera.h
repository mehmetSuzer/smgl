
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <color.h>
#include <vector3d.h>

typedef struct {
    const Vector3D& position;
    const Color& color;
} Light;

class Camera {
private:
    const Vector3D position;
    const float near;
    const float far;

    Vector3D lower_left;
    Vector3D right_per_x;
    Vector3D up_per_y;

    float screen_half_width;
    float screen_half_height;

public:
    Camera(const Vector3D& position_, const Vector3D& direction_, const Vector3D& up_, 
        float near_, float far_, float FOV_radian_, uint32_t width_, uint32_t height_);
        
    const Vector3D& getPosition(void) const;
    float getNear(void) const;
    float getFar(void) const;

    Ray generateRay(float x, float y) const;
};

#endif // __CAMERA_H__
