
#include "sphere.h"

Sphere::Sphere(): Shape(SPHERE, Color("WHITE"), 0.0, 0.0, 1.0, 0.0, false) {
    center = Vector3D();
    radius = 1.0f;
}

Sphere::Sphere(const Vector3D& center, float radius, const Color& color, float diffuse_coef, 
    float specular_coef, float specular_pow, float ambient_coef, bool reflect): 
    Shape(SPHERE, color, diffuse_coef, specular_coef, specular_pow, ambient_coef, reflect) {

    Sphere::center = center;
    Sphere::radius = (radius > 0.0f) ? radius : 1.0f;
}

bool Sphere::intersect(Intersect *intersect, const Vector3D& source, const Vector3D& ray) const {
    float dot_result = (source-center).dot(ray);
    float discr = 4.0f * (dot_result*dot_result - (source-center).mag_square() + radius*radius);
    if (discr > 0.0f) { // ray intersect the sphere
        float t = -dot_result - std::sqrt(discr) * 0.5f;
        if (t <= 0.0f) { // in the opposite side of the ray direction
            return false;
        }
        if (intersect != NULL) {
            // camera.position + t*unit = (the point where the ray intersects the sphere)
            // choose the smaller t
            intersect->t = t;
            intersect->hit_location = source + ray * t;
            intersect->normal = (intersect->hit_location - center).normalize();
        }
        return true;
    } else { // ray does not intersect the sphere
        return false;
    }
}   