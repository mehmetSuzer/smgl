
#include "mesh.h"

Mesh::Mesh(const std::vector<Shape*>& shapes_) : Shape(Color::Black, 0.0f, 0.0f, VACUUM_REFRACTIVE_INDEX), shapes(shapes_) {
    Vector3D minPoint;
    Vector3D maxPoint;
    findAABBMinMaxPoints(minPoint, maxPoint);
    boundingVolume.setMinPoint(minPoint);
    boundingVolume.setMaxPoint(maxPoint);
}

bool Mesh::intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const {
    if (!boundingVolume.intersect(NULL, NULL, ray, far)) {
        return false;
    }

    if (intersect == NULL) {
        for (uint32_t i = 0; i < shapes.size(); i++) {
            if (shapes[i]->intersect(NULL, intersectedShape, ray, far)) {
                return true;
            }
        }
    } else {
        Intersect closestIntersect = {.t = INFINITY};
        Intersect currentIntersect;
        Shape* closestShape = NULL;
        Shape* currentShape;

        for (uint32_t i = 0; i < shapes.size(); i++) {
            if (shapes[i]->intersect(&currentIntersect, &currentShape, ray, far) && currentIntersect.t < closestIntersect.t) {
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

void Mesh::findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const {
    minPoint = Vector3D(INFINITY);
    maxPoint = Vector3D(-INFINITY);

    Vector3D currentMinPoint;
    Vector3D currentMaxPoint;

    for (uint32_t i = 0; i < shapes.size(); i++) {
        shapes[i]->findAABBMinMaxPoints(currentMinPoint, currentMaxPoint);

        if (currentMinPoint.x < minPoint.x) {
            minPoint.x = currentMinPoint.x;
        } 
        if (currentMinPoint.y < minPoint.y) {
            minPoint.y = currentMinPoint.y;
        } 
        if (currentMinPoint.z < minPoint.z) {
            minPoint.z = currentMinPoint.z;
        } 

        if (currentMaxPoint.x > maxPoint.x) {
            maxPoint.x = currentMaxPoint.x;
        }
        if (currentMaxPoint.y > maxPoint.y) {
            maxPoint.y = currentMaxPoint.y;
        }
        if (currentMaxPoint.z > maxPoint.z) {
            maxPoint.z = currentMaxPoint.z;
        }
    }
}
