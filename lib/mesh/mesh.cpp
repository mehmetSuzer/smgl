
#include "mesh.h"

Mesh::Mesh(const std::vector<Shape*>& shapes_, const Shape* boundingVolume_)
    : Shape(Color::Black, 0.0f, 0.0f, VACUUM_REFRACTIVE_INDEX), shapes(shapes_), boundingVolume(boundingVolume_) {}

bool Mesh::intersect(Intersect* intersect, const Ray& ray, float far) const {
    if (!boundingVolume->intersect(NULL, ray, far)) {
        return false;
    }

    Intersect closestIntersect = {.t = INFINITY};
    Intersect currentIntersect;
    Shape* closestShape = NULL;
    for (uint32_t i = 0; i < shapes.size(); i++) {
        if (shapes[i]->intersect(&currentIntersect, ray, far) && currentIntersect.t < closestIntersect.t) {
            closestIntersect = currentIntersect;
            closestShape = shapes[i];
        }
    }

    if (closestShape != NULL) {
        if (intersect != NULL) {
            *intersect = closestIntersect;
            intersect->shape = closestShape;
        }
        return true;
    }

    return false;
}
