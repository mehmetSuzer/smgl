
#include "mesh.h"

Mesh::Mesh(const std::vector<Shape*>& shapes_, const Shape* boundingVolume_)
    : Shape(Color::Black, 0.0f, 0.0f, VACUUM_REFRACTIVE_INDEX), shapes(shapes_), boundingVolume(boundingVolume_) {}

bool Mesh::intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const {
    if (!boundingVolume->intersect(NULL, NULL, ray, far)) {
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
