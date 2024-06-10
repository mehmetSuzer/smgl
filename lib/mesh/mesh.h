
#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <aabb.h>

class Mesh : public Shape {
private:
    const std::vector<Shape*>& shapes;
    AABB boundingVolume;

public:
    Mesh(const std::vector<Shape*>& shapes_);

    bool intersect(Intersect* intersect, Shape** intersectedShape, const Ray& ray, float far) const override;
    void findAABBMinMaxPoints(Vector3D& minPoint, Vector3D& maxPoint) const override;
};

#endif // __MESH_H__
