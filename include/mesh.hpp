#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "Vector2f.h"
#include "Vector3f.h"
#include "bvh.hpp"


class Mesh : public Object3D {

public:
    Mesh(const char *filename, Material *m);

    struct TriangleIndex {
        TriangleIndex() {
            x[0] = 0; x[1] = 0; x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v;
    std::vector<Vector2f> vt;
    std::vector<TriangleIndex> t;
    std::vector<TriangleIndex> tex;
    std::vector<Vector3f> n;
    bool intersect(const Ray &r, Hit &h, float tmin) const override;
    bool intersectP(const Ray &r, float tmin, float tmax, const Object3D* ignore) const override;
    Bound3f Bounds() const override;

private:

    BVH* bvh;

    // Normal can be used for light estimation
    void computeNormal();
};

#endif
