
#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <shape.h>

class Mesh : public Shape {
private:
    const std::vector<Shape*>& shapes;
    const Shape* boundingVolume;

public:
    Mesh(const std::vector<Shape*>& shapes_, const Shape* boundingVolume_);

    bool intersect(Intersect* intersect, const Ray& ray, float far) const override;
};

#endif // __MESH_H__
