
#include "sphere.h"

Sphere::Sphere(): Shape(Color::Black, false, 0.0f, VACUUM_REFRACTIVE_INDEX), radius(1.0f) {}

Sphere::Sphere(const Vector3D& center_, float radius_, const Color& color, bool reflect, float transparency, float refractiveIndex): 
    Shape(color, reflect, transparency, refractiveIndex), center(center_), radius(radius_) {
    assert(radius_ > 0.0f);
}

// Checks whether the ray intersects the sphere and finds the intersection details
bool Sphere::intersect(Intersect* intersect, const Ray& ray, float far) const {
    const Vector3D centerToOrigin = ray.origin - center;
    const float dotProduct = centerToOrigin.dot(ray.dir);
    const float quarterDiscriminant = dotProduct*dotProduct - centerToOrigin.magSquare() + radius*radius;
    if (quarterDiscriminant > 0.0f) { // The ray intersects the sphere
        float t = -dotProduct - sqrtf(quarterDiscriminant); // Choose the closer intersection first
        bool rayOriginIsInSphere = false;
        if (t >= far) { // Check whether the ray is in the allowed range
            return false;
        } else if (t <= 0.0f) {
            t = -dotProduct + sqrtf(quarterDiscriminant); // Choose the further intersection 
            if (t >= far || t <= 0.0f) {
                return false;
            }
            rayOriginIsInSphere = true;
        }
        if (intersect != NULL) {
            intersect->t = t;
            intersect->hitLocation = ray.origin + ray.dir * t;
            intersect->normal = (intersect->hitLocation - center).normalize();
            if (rayOriginIsInSphere) {
                intersect->normal *= -1.0f;
            }
        }
        return true;
    } else { // ray does not intersect the sphere
        return false;
    }
}   
