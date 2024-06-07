
#include "triangle.h"

Triangle::Triangle(): Shape(Color::Black, 0.0f, 0.0f, VACUUM_REFRACTIVE_INDEX) {}
    
Triangle::Triangle(const Vector3D& a_, const Vector3D& b_, const Vector3D& c_, const Color& color, 
    float reflectivity, float transparency, float refractiveIndex) 
    : Shape(color, reflectivity, transparency, refractiveIndex), a(a_), b(b_), c(c_), normal((b_-a_).cross(c_-a_).normalize()) {}

// Checks whether the ray intersects the triangle and finds the intersection details
bool Triangle::intersect(Intersect* intersect, const Ray& ray, float far) const {
    // Check whether the ray direction is parallel to the triangle
    if (abs(normal.dot(ray.dir)) < EPSILON6) {
        return false;
    }

    // Solve for Beta, Gamma, and t 
    Matrix3x3 matrix = Matrix3x3(a-b, a-c, ray.dir, true);
    Vector3D vector = a - ray.origin;
    const Vector3D result = Matrix3x3::solve(matrix, vector);

    // If Beta > 0 and Gamma > 0, Beta+Gamma < 1, t > 0, and t < far, the ray intersects the triangle
    if (result.x > EPSILON6 && result.y > EPSILON6 && result.x + result.y < 1.0f && result.z > EPSILON6 && result.z < far) {
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
