
#include "sphere.h"

Sphere::Sphere(): Shape(Color::Black, false, true, VACUUM_REFRACTIVE_INDEX), radius(1.0f) {}

Sphere::Sphere(const Vector3D& center_, float radius_, const Color& color, bool reflect, float transparency, float refractive_index): 
    Shape(color, reflect, transparency, refractive_index), center(center_), radius(radius_) {
    assert(radius_ > 0.0f);
}

bool Sphere::intersect(Intersect* intersect, const Ray& ray) const {
    const Vector3D center_to_origin = ray.origin - center;
    const float dot_result = center_to_origin.dot(ray.dir);
    const float discr = 4.0f * (dot_result*dot_result - center_to_origin.mag_square() + radius*radius);
    if (discr > 0.0f) { // the ray intersects the sphere
        const float t = -(dot_result + sqrtf(discr) * 0.5f);
        if (t <= 0.0f) { // in the opposite side of the ray direction
            return false;
        }
        if (intersect != NULL) {
            intersect->t = t;
            intersect->hit_location = ray.origin + ray.dir * t;
            intersect->normal = (intersect->hit_location - center).normalize();
        }
        return true;
    } else { // ray does not intersect the sphere
        return false;
    }
}   
