
#include "bezier.h"

BezierSurface::BezierSurface() : Shape(Color::Black, false, 0.0f, VACUUM_REFRACTIVE_INDEX), subdivision(4) {}

BezierSurface::BezierSurface(Vector3D* controls, uint32_t subdivision_, const Color& color, bool reflect, float transparency, float refractiveIndex) 
    : Shape(color, reflect, transparency, refractiveIndex), subdivision(subdivision_) {
    findBoundingVolume(controls);

    const uint32_t vertexNumber = (subdivision+1) * (subdivision+1);
    std::vector<Vector3D> vertices(vertexNumber);
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

            triangles.push_back(Triangle(vertex3, vertex2, vertex1, color, reflect, transparency, refractiveIndex));
            triangles.push_back(Triangle(vertex3, vertex4, vertex2, color, reflect, transparency, refractiveIndex));
        }
    }
}

// Calculates B(u) and B(v) for surface function
void BezierSurface::generateControlPointScalars(float* xVector, float x) const {
    assert(0.0f <= x && x <= 1.0f);
    const float oneMinusX = 1.0f - x;

    xVector[0] = oneMinusX * oneMinusX * oneMinusX;
    xVector[1] = 3.0f * oneMinusX * oneMinusX * x;
    xVector[2] = 3.0f * oneMinusX * x * x;
    xVector[3] = x * x * x;
}

// Returns the surface point for given control points, u, and v
Vector3D BezierSurface::getPoint(const Vector3D* controls, float u, float v) const {
    float uVector[4];
    float vVector[4];
    generateControlPointScalars(uVector, u);
    generateControlPointScalars(vVector, v);

    Vector3D point = Vector3D();
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            point += controls[(i<<2)+j] * (uVector[i] * vVector[j]);
        }
    }

    return point;
}

// Finds the min and max points of the Axis Aligned Bouding Box of the surface
void BezierSurface::findBoundingVolume(const Vector3D* controls) {
    Vector3D minPoint = Vector3D(INFINITY, INFINITY, INFINITY);
    Vector3D maxPoint = Vector3D(-INFINITY, -INFINITY, -INFINITY);

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

    boundingVolume.setMinPoint(minPoint);
    boundingVolume.setMaxPoint(maxPoint);
}

// Checks whether the ray intersects the surface and finds the intersection details
bool BezierSurface::intersect(Intersect* intersect, const Ray& ray, float far) const {
    if (!boundingVolume.intersect(NULL, ray, far)) {
        return false;
    }

    for (uint32_t i = 0; i < triangles.size(); i++) {
        if (triangles[i].intersect(intersect, ray, far)) {
            return true;
        }
    }
    return false;
}
