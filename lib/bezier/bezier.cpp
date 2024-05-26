
#include "bezier.h"

BezierSurface::BezierSurface() : Shape(Color::Black, false, true, VACUUM_REFRACTIVE_INDEX), subdivision(4) {}

BezierSurface::BezierSurface(Vector3D* controls, uint32_t subdivision_, const Color& color, bool reflect, float transparency, float refractive_index) 
    : Shape(color, reflect, transparency, refractive_index), subdivision(subdivision_) {
    findAABB(controls);

    const uint32_t vertex_number = (subdivision+1) * (subdivision+1);
    std::vector<Vector3D> vertices(vertex_number);
    uint32_t index = 0;

    // Sample (subdivision X subdivision) many surfaces
    for (uint32_t i = 0; i <= subdivision; i++) {
        float u = static_cast<float>(i) / subdivision;
        for (uint32_t j = 0; j <= subdivision; j++) {
            float v = static_cast<float>(j) / subdivision;
            vertices[index++] = getPoint(controls, u, v); 
        }
    }

    // Triangulate the surfaces
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

// Calculates B(u) and B(v) for surface function
void BezierSurface::generateControlPointScalars(float* x_vector, float x) const {
    assert(0.0f <= x && x <= 1.0f);
    const float one_minus_x = 1.0f - x;

    x_vector[0] = one_minus_x * one_minus_x * one_minus_x;
    x_vector[1] = 3.0f * one_minus_x * one_minus_x * x;
    x_vector[2] = 3.0f * one_minus_x * x * x;
    x_vector[3] = x * x * x;
}

// Returns the surface point for given control points, u, and v
Vector3D BezierSurface::getPoint(const Vector3D* controls, float u, float v) const {
    float u_vector[4];
    float v_vector[4];
    generateControlPointScalars(u_vector, u);
    generateControlPointScalars(v_vector, v);

    Vector3D point = Vector3D();
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            point += controls[(i<<2)+j] * (u_vector[i] * v_vector[j]);
        }
    }

    return point;
}

// Finds the min and max points of the Axis Aligned Bouding Box of the surface
void BezierSurface::findAABB(const Vector3D* controls) {
    for (uint32_t i = 0; i < 16; i++) {
        if (controls[i].x < minPoint.x) {
            minPoint.x = controls[i].x;
        } else if (controls[i].x > maxPoint.x) {
            maxPoint.x = controls[i].x;
        }

        if (controls[i].y < minPoint.y) {
            minPoint.y = controls[i].y;
        } else if (controls[i].y > maxPoint.y) {
            maxPoint.y = controls[i].y;
        }

        if (controls[i].z < minPoint.z) {
            minPoint.z = controls[i].z;
        } else if (controls[i].z > maxPoint.z) {
            maxPoint.z = controls[i].z;
        }
    }
}

// Checks whether the ray intersects the AABB
bool BezierSurface::intersectAABB(const Ray& ray) const {
    const Vector3D min_x_divide_dir_x = minPoint.divide(ray.dir);
    const Vector3D max_x_divide_dir_x = maxPoint.divide(ray.dir);
    const Vector3D origin_divide_dir_x = ray.origin.divide(ray.dir);

    Vector3D t1 = min_x_divide_dir_x - origin_divide_dir_x;
    Vector3D t2 = max_x_divide_dir_x - origin_divide_dir_x;

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

    const float small_t = greater(greater(t1.x, t1.y), t1.z);
    const float great_t = smaller(smaller(t2.x, t2.y), t2.z);
    
    return small_t <= great_t;
}

// Checks whether the ray intersects the surface and finds the intersection details
bool BezierSurface::intersect(Intersect* intersect, const Ray& ray) const {
    if (!intersectAABB(ray)) {
        return false;
    }

    for (uint32_t i = 0; i < triangles.size(); i++) {
        if (triangles[i].intersect(intersect, ray)) {
            return true;
        }
    }
    return false;
}
