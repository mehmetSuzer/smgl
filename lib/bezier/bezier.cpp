
#include "bezier.h"

BezierSurface::BezierSurface() : Shape(Color::Black, 0.0f, 0.0f, VACUUM_REFRACTIVE_INDEX), subdivision(4) {}

BezierSurface::BezierSurface(const Vector3D* controls, uint32_t subdivision_, const Color& color, float reflectivity, float transparency, float refractiveIndex) 
    : Shape(color, reflectivity, transparency, refractiveIndex), subdivision(subdivision_) {
        
    findBoundingVolume(controls);
    Vector3D vertices[(subdivision+1) * (subdivision+1)];
    const float dx = 1.0f / subdivision;
    uint32_t index = 0;

    // Sample the vertices of (subdivision X subdivision) many surfaces
    for (float u = 0; u <= 1.0f; u += dx) {
        for (float v = 0; v <= 1.0f; v += dx) {
            vertices[index++] = getPoint(controls, u, v); 
        }
    }

    // Triangulate the surfaces
    index = 0;
    for (uint32_t i = 0; i < subdivision; i++) {
        for (uint32_t j = 0; j < subdivision; j++) {
            const Vector3D vertex1 = vertices[index+j];
            const Vector3D vertex2 = vertices[index+j+1];
            const Vector3D vertex3 = vertices[index+subdivision+j+1];
            const Vector3D vertex4 = vertices[index+subdivision+j+2];

            triangles.push_back(Triangle(vertex3, vertex2, vertex1, color, reflectivity, transparency, refractiveIndex));
            triangles.push_back(Triangle(vertex3, vertex4, vertex2, color, reflectivity, transparency, refractiveIndex));
        }
        index += subdivision+1;
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

    Vector3D point = Vector3D(0.0f, 0.0f, 0.0f);
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
        } 
        if (controls[i].x > maxPoint.x) {
            maxPoint.x = controls[i].x;
        }

        if (controls[i].y < minPoint.y) {
            minPoint.y = controls[i].y;
        } 
        if (controls[i].y > maxPoint.y) {
            maxPoint.y = controls[i].y;
        }

        if (controls[i].z < minPoint.z) {
            minPoint.z = controls[i].z;
        } 
        if (controls[i].z > maxPoint.z) {
            maxPoint.z = controls[i].z;
        }
    }

    boundingVolume.setMinPoint(minPoint);
    boundingVolume.setMaxPoint(maxPoint);
}

// Checks whether the ray intersects the surface and finds the intersection details
bool BezierSurface::intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const {
    if (!boundingVolume.intersect(NULL, NULL, ray, far)) {
        return false;
    }

    if (intersect == NULL) {
        for (uint32_t i = 0; i < triangles.size(); i++) {
            if (triangles[i].intersect(NULL, intersectedShape, ray, far)) {
                return true;
            }
        }
    } else {
        Intersect closestIntersect = {.t = INFINITY};
        Intersect currentIntersect;
        Shape* closestShape = NULL;
        Shape* currentShape;

        for (uint32_t i = 0; i < triangles.size(); i++) {
            if (triangles[i].intersect(&currentIntersect, &currentShape, ray, far) && currentIntersect.t < closestIntersect.t) {
                closestIntersect = currentIntersect;
                closestShape = currentShape;
            }
        }
        
        if (closestShape != NULL) {
            *intersect = closestIntersect;
            *intersectedShape = closestShape;
            return true;
        } 
    }

    return false;
}
