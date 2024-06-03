
#include "triangle.h"

Triangle::Triangle(): Shape(Color::Black, false, 0.0f, VACUUM_REFRACTIVE_INDEX) {}
    
Triangle::Triangle(const Vector3D& a_, const Vector3D& b_, const Vector3D& c_, const Color& color, bool reflect, float transparency, float refractive_index):
    Shape(color, reflect, transparency, refractive_index), a(a_), b(b_), c(c_), normal((b_-a_).cross(c_-a_).normalize()) {}

// Checks whether the ray intersects the triangle and finds the intersection details
bool Triangle::intersect(Intersect* intersect, const Ray& ray) const {
    // Solve for Beta, Gamma, and t 
    Matrix3x3 matrix = Matrix3x3(a-b, a-c, ray.dir, true);
    Vector3D vector = a - ray.origin;
    const Vector3D result = Matrix3x3::solve(matrix, vector);

    // If Beta > 0 and Gamma > 0, Beta+Gamma < 1, and t > 0, then the ray intersects the triangle
    if (result.x > 0.0f && result.y > 0.0f && result.x+result.y < 1.0f && result.z > 0.0f) {
        if (intersect != NULL) {
            intersect->t = result.z;
            intersect->hit_location = ray.origin + ray.dir * intersect->t;
            if (ray.dir.dot(normal) < 0.0f) {
                intersect->normal = normal;
            } else {
                intersect->normal = -normal;
            }
        }
        return true;
    } else { // ray does not intersect the triangle
        return false;
    }
}
