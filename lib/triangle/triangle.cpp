
#include "triangle.h"

Triangle::Triangle(): Shape(TRIANGLE, Color("BLACK"), 0.0, 0.0, 1.0, 0.0, false) {
    a = Vector3D();
    b = Vector3D();
    c = Vector3D();
    normal = Vector3D();
}
    
Triangle::Triangle(const Vector3D& a, const Vector3D& b, const Vector3D& c, const Color& color,
    float diffuse_coef, float specular_coef, float specular_pow, float ambient_coef, bool reflect):
    Shape(TRIANGLE, color, diffuse_coef, specular_coef, specular_pow, ambient_coef, reflect) {

    Triangle::a = a;
    Triangle::b = b;
    Triangle::c = c;
    normal = (b-a).cross(c-a).normalize();
}

bool Triangle::intersect(Intersect *intersect, const Vector3D& source, const Vector3D& ray) const {
    // Calculate Beta, Gamma, and t with Cramer's Rule
    Matrix3x3 matrix(a-b, a-c, ray, true);
    Vector3D result = matrix.cramers_rule(a-source);
    // If Beta < 0 and Gamma > 0, Beta+Gamma < 1, and t > 0, then the ray intersects the triangle
    if (result.x > 0.0f && result.y > 0.0f && result.x+result.y < 1.0f && result.z > 0.0f) {
        if (intersect != NULL) {
            intersect->t = result.z;
            intersect->hit_location = source + ray * intersect->t;
            if (ray.dot(normal) < 0.0f) {
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
