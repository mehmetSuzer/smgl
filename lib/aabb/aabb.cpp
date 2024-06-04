
#include "aabb.h"

AABB::AABB() : Shape(Color::Black, false, 0.0f, VACUUM_REFRACTIVE_INDEX) {}

AABB::AABB(const Vector3D& minPoint_, const Vector3D& maxPoint_, const Color& color, bool reflect, float transparency, float refractiveIndex) 
    : Shape(color, reflect, transparency, refractiveIndex), minPoint(minPoint_), maxPoint(maxPoint_) {
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

    const Vector3D minPointDivideRayDir = Vector3D(
        perpendicularToX ? -INFINITY : minPoint.x / ray.dir.x,
        perpendicularToY ? -INFINITY : minPoint.y / ray.dir.y,
        perpendicularToZ ? -INFINITY : minPoint.z / ray.dir.z
    );
    const Vector3D maxPointDivideRayDir = Vector3D(
        perpendicularToX ? INFINITY : maxPoint.x / ray.dir.x,
        perpendicularToY ? INFINITY : maxPoint.y / ray.dir.y,
        perpendicularToZ ? INFINITY : maxPoint.z / ray.dir.z
    );
    const Vector3D rayOriginDivideRayDir = Vector3D(
        perpendicularToX ? 0.0f : ray.origin.x / ray.dir.x,
        perpendicularToY ? 0.0f : ray.origin.y / ray.dir.y,
        perpendicularToZ ? 0.0f : ray.origin.z / ray.dir.z
    );

    Vector3D t1 = minPointDivideRayDir - rayOriginDivideRayDir;
    Vector3D t2 = maxPointDivideRayDir - rayOriginDivideRayDir;

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

    const float lowT = greater(greater(t1.x, t1.y), t1.z);
    const float highT = smaller(smaller(t2.x, t2.y), t2.z);

    bool rayOriginIsInAABB = false;
    float t;

    if (lowT >= highT || lowT >= far) { // If the interval for t is empty or the closer intersection is out of range
        return false;
    } else if (lowT > 0.0f) { // The intersection occurs in the forward direction of the ray
        t = lowT;
    } else if (highT <= 0.0f || highT >= far) { // The AABB is behind the object or the intersection is out range
        return false;
    } else { // Origin of the ray is in the AABB
        t = highT;
        rayOriginIsInAABB = true;
    }

    if (intersect != NULL) {
        intersect->t = t;
        intersect->hitLocation = ray.origin + ray.dir * t;

        const Vector3D minDifference = intersect->hitLocation - minPoint;
        const Vector3D maxDifference = intersect->hitLocation - maxPoint;

        intersect->normal = (-EPSILON4 < minDifference.x && minDifference.x < EPSILON4) ? Vector3D::left : 
                            (-EPSILON4 < minDifference.y && minDifference.y < EPSILON4) ? Vector3D::down : 
                            (-EPSILON4 < minDifference.z && minDifference.z < EPSILON4) ? Vector3D::backward : 
                            (-EPSILON4 < maxDifference.x && maxDifference.x < EPSILON4) ? Vector3D::right : 
                            (-EPSILON4 < maxDifference.y && maxDifference.y < EPSILON4) ? Vector3D::up : 
                                                                                          Vector3D::forward; 

        if (rayOriginIsInAABB) {
            intersect->normal *= -1.0f;
        }
    }
    return true;
}
