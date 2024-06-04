
#include "aabb.h"

AABB::AABB() : Shape(Color::Black, false, 0.0f, VACUUM_REFRACTIVE_INDEX) {}

AABB::AABB(const Vector3D& minPoint_, const Vector3D& maxPoint_, const Color& color, bool reflect, float transparency, float refractive_index) 
    : Shape(color, reflect, transparency, refractive_index), minPoint(minPoint_), maxPoint(maxPoint_) {
    assert(minPoint.x < maxPoint.x);
    assert(minPoint.y < maxPoint.y);
    assert(minPoint.z < maxPoint.z);
}

void AABB::setMinPoint(const Vector3D& point) {
    minPoint = point;
}

void AABB::setMaxPoint(const Vector3D& point) {
    maxPoint = point;
}

bool AABB::intersect(Intersect* intersect, const Ray& ray, float far) const {
    // If the ray is perpendicular to an axis, then don't use this axis to calculate t
    const bool perpendicularToX = (ray.dir.x == 0.0f); 
    const bool perpendicularToY = (ray.dir.y == 0.0f); 
    const bool perpendicularToZ = (ray.dir.z == 0.0f); 

    // Check whether the ray is parallel to an axis and misses the box
    if ((perpendicularToX && !(minPoint.x < ray.origin.x && ray.origin.x < maxPoint.x)) || 
        (perpendicularToY && !(minPoint.y < ray.origin.y && ray.origin.y < maxPoint.y)) || 
        (perpendicularToZ && !(minPoint.z < ray.origin.z && ray.origin.z < maxPoint.z))) {
        return false;
    }

    const Vector3D min_divide_dir = Vector3D(
        perpendicularToX ? -INFINITY : minPoint.x / ray.dir.x,
        perpendicularToY ? -INFINITY : minPoint.y / ray.dir.y,
        perpendicularToZ ? -INFINITY : minPoint.z / ray.dir.z
    );
    const Vector3D max_divide_dir = Vector3D(
        perpendicularToX ? INFINITY : maxPoint.x / ray.dir.x,
        perpendicularToY ? INFINITY : maxPoint.y / ray.dir.y,
        perpendicularToZ ? INFINITY : maxPoint.z / ray.dir.z
    );
    const Vector3D origin_divide_dir = Vector3D(
        perpendicularToX ? 0.0f : ray.origin.x / ray.dir.x,
        perpendicularToY ? 0.0f : ray.origin.y / ray.dir.y,
        perpendicularToZ ? 0.0f : ray.origin.z / ray.dir.z
    );

    Vector3D t1 = min_divide_dir - origin_divide_dir;
    Vector3D t2 = max_divide_dir - origin_divide_dir;

    if (t2.x < t1.x) {
        const float temp = t1.x;
        t1.x = t2.x;
        t2.x = temp;
    }
    if (t2.y < t1.y) {
        const float temp = t1.y;
        t1.y = t2.y;
        t2.y = temp;
    }
    if (t2.z < t1.z) {
        const float temp = t1.z;
        t1.z = t2.z;
        t2.z = temp;
    }

    const float low_t = greater(greater(t1.x, t1.y), t1.z);
    const float high_t = smaller(smaller(t2.x, t2.y), t2.z);

    bool rayOriginIsInAABB = false;
    float t;

    if (low_t >= high_t || low_t >= far) { // If the interval for t is empty or the closer intersection is out of range
        return false;
    } else if (low_t > 0.0f) { // The intersection occurs in the forward direction of the ray
        t = low_t;
    } else if (high_t <= 0.0f || high_t >= far) { // The AABB is behind the object or the intersection is out range
        return false;
    } else { // Origin of the ray is in the AABB
        t = high_t;
        rayOriginIsInAABB = true;
    }

    if (intersect != NULL) {
        intersect->t = t;
        intersect->hit_location = ray.origin + ray.dir * t;

        const Vector3D minDifference = intersect->hit_location - minPoint;
        const Vector3D maxDifference = intersect->hit_location - maxPoint;

        intersect->normal = (-EPSILON < minDifference.x && minDifference.x < EPSILON) ? Vector3D::left : 
                            (-EPSILON < minDifference.y && minDifference.y < EPSILON) ? Vector3D::down : 
                            (-EPSILON < minDifference.z && minDifference.z < EPSILON) ? Vector3D::backward : 
                            (-EPSILON < maxDifference.x && maxDifference.x < EPSILON) ? Vector3D::right : 
                            (-EPSILON < maxDifference.y && maxDifference.y < EPSILON) ? Vector3D::up : 
                                                                                        Vector3D::forward; 

        if (rayOriginIsInAABB) {
            intersect->normal *= -1.0f;
        }
    }
    return true;
}
