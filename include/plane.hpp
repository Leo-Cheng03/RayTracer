#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m), normal(normal), d(d) {
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) const override {
        float nd = Vector3f::dot(normal, r.getDirection());
        if (nd == 0) return false;
        float t = (d - Vector3f::dot(normal, r.getOrigin())) / nd;
        if (t < tmin || t > h.getT()) return false;
        h.set(t, material, normal);
        return true;
    }

    bool intersectP(const Ray &r, float tmin, float tmax, const Object3D* ignore) const override {
        if (ignore == this) return false;
        float nd = Vector3f::dot(normal, r.getDirection());
        if (nd == 0) return false;
        float t = (d - Vector3f::dot(normal, r.getOrigin())) / nd;
        return t >= tmin && t <= tmax;
    }

    Bound3f Bounds() const override {
        return Bound3f(Vector3f(-INFINITY, -INFINITY, -INFINITY), Vector3f(INFINITY, INFINITY, INFINITY));
    }

protected:
    Vector3f normal;
    float d;
};

#endif //PLANE_H
		

