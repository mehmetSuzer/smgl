
#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include <shape.h>

class Mesh {
private:
    std::vector<Shape*> shapes;
    Shape* boundingVolume;

public:
    Mesh(const std::vector<Shape*>& shapes_, Shape* boundingVolume_);

};

#endif // __MESH_H__
