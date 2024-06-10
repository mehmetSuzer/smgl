
#include "triangle.h"

Triangle::Triangle(): Shape(Color::Black, 0.0f, 0.0f, VACUUM_REFRACTIVE_INDEX) {}
    
Triangle::Triangle(const Vector3D& a, const Vector3D& b, const Vector3D& c, 
    const Color& color, float reflectivity, float transparency, float refractiveIndex) 
    : Shape(color, reflectivity, transparency, refractiveIndex) {
    
    points[0] = a;
    points[1] = b;
    points[2] = c;
    normal = (b-a).cross(c-a).normalize();
}

// Checks whether the ray intersects the triangle and finds the intersection details
bool Triangle::intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const {
    // Check whether the ray direction is parallel to the triangle
    if (abs(normal.dot(ray.dir)) < EPSILON6) {
        return false;
    }

    // Solve for Beta, Gamma, and t 
    Matrix3x3 matrix = Matrix3x3(points[0]-points[1], points[0]-points[2], ray.dir, true);
    Vector3D vector = points[0] - ray.origin;
    const Vector3D result = Matrix3x3::solve(matrix, vector);

    // If Beta > 0 and Gamma > 0, Beta + Gamma < 1, t > 0, and t < far, the ray intersects the triangle
    if (result.x > EPSILON6 && result.y > EPSILON6 && result.x + result.y < 1.0f && result.z > EPSILON6 && result.z < far) {
        if (intersectedShape != NULL) {
            *intersectedShape = (Shape*)this;
        }
        if (intersect != NULL) {
            intersect->t = result.z;
            intersect->hitLocation = ray.origin + ray.dir * intersect->t;
            intersect->normal = normal;
            if (ray.dir.dot(normal) > 0.0f) {
                intersect->normal *= -1.0f;
            }
        }
        return true;
    } else { // ray does not intersect the triangle
        return false;
    }
}

void Triangle::findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const {
    minPoint = Vector3D(INFINITY);
    maxPoint = Vector3D(-INFINITY);

    for (uint32_t i = 0; i < 3; i++) {
        if (points[i].x < minPoint.x) {
            minPoint.x = points[i].x;
        } 
        if (points[i].x > maxPoint.x) {
            maxPoint.x = points[i].x;
        }

        if (points[i].y < minPoint.y) {
            minPoint.y = points[i].y;
        } 
        if (points[i].y > maxPoint.y) {
            maxPoint.y = points[i].y;
        }

        if (points[i].z < minPoint.z) {
            minPoint.z = points[i].z;
        } 
        if (points[i].z > maxPoint.z) {
            maxPoint.z = points[i].z;
        }
    }
}
