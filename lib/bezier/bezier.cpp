
#include "bezier.h"

BezierSurface::BezierSurface() : Shape(Color("BLACK"), false, true, VACUUM_REFRACTIVE_INDEX) {}

BezierSurface::BezierSurface(Vector3D* controls, const Color& color, bool reflect, float transparency, float refractive_index) 
    : Shape(color, reflect, transparency, refractive_index) {
    // Sample (subdivision X subdivision) many points on the surface
    for (uint32_t i = 0; i <= subdivision; i++) {
        float u = static_cast<float>(i) / subdivision;
        for (uint32_t j = 0; j <= subdivision; j++) {
            float v = static_cast<float>(j) / subdivision;
            vertices.push_back(getPoint(controls, u, v)); 
        }
    }

    // Triangulate the surface
    for (uint32_t i = 0; i < subdivision; i++) {
        for (uint32_t j = 0; j < subdivision; j++) {
            Vector3D vertex1 = vertices[i*(subdivision+1)+j];
            Vector3D vertex2 = vertices[i*(subdivision+1)+j+1];
            Vector3D vertex3 = vertices[(i+1)*(subdivision+1)+j];
            Vector3D vertex4 = vertices[(i+1)*(subdivision+1)+j+1];

            triangles.push_back(Triangle(vertex3, vertex2, vertex1, color, reflect, transparency, refractive_index));
            triangles.push_back(Triangle(vertex3, vertex4, vertex2, color, reflect, transparency, refractive_index));
        }
    }
}

// Used to calculate B(u) and B(v) for surface function
void BezierSurface::generateControlPointScalars(float* x_vector, float x) const {
    assert(0.0f <= x && x <= 1.0f);
    const float one_minus_x = 1.0f - x;

    x_vector[0] = one_minus_x * one_minus_x * one_minus_x;
    x_vector[1] = 3.0f * one_minus_x * one_minus_x * x;
    x_vector[2] = 3.0f * one_minus_x * x * x;
    x_vector[3] = x * x * x;
}

// Returns the surface point for given control points, u, and v
Vector3D BezierSurface::getPoint(Vector3D* controls, float u, float v) const {
    float u_vector[4];
    float v_vector[4];
    generateControlPointScalars(u_vector, u);
    generateControlPointScalars(v_vector, v);

    Vector3D point = Vector3D();
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            point += controls[4*i+j] * (u_vector[i] * v_vector[j]);
        }
    }

    return point;
}

// Checks whether the ray intersects a triangle 
bool BezierSurface::intersect(Intersect *intersect, const Ray& ray) const {
    for (uint32_t i = 0; i < triangles.size(); i++) {
        if (triangles[i].intersect(intersect, ray)) {
            return true;
        }
    }
    return false;
}
