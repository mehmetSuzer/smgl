
#include "triangle.h"

Triangle::Triangle(): Shape(Color("BLACK"), false) {}
    
Triangle::Triangle(const Vector3D& a_, const Vector3D& b_, const Vector3D& c_, const Color& color, bool reflect):
    Shape(color, reflect), a(a_), b(b_), c(c_), normal((b_-a_).cross(c_-a_).normalize()) {}

bool Triangle::intersect(Intersect *intersect, const Ray& ray) const {
    // Calculate Beta, Gamma, and t with Cramer's Rule
    Matrix3x3 matrix(a-b, a-c, ray.dir, true);
    Vector3D result = matrix.cramers_rule(a-ray.source);
    // If Beta < 0 and Gamma > 0, Beta+Gamma < 1, and t > 0, then the ray intersects the triangle
    if (result.x > 0.0f && result.y > 0.0f && result.x+result.y < 1.0f && result.z > 0.0f) {
        if (intersect != NULL) {
            intersect->t = result.z;
            intersect->hit_location = ray.source + ray.dir * intersect->t;
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
