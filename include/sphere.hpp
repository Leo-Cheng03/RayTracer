#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement functions and add more fields as necessary

class Sphere : public Object3D {
public:
    Sphere() : center(Vector3f(0, 0, 0)), radius(1), Object3D(nullptr){
        // unit ball at the center
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        float a = r.getDirection().squaredLength();
        float b = 2 * Vector3f::dot(r.getDirection(), r.getOrigin() - center);
        float c = (r.getOrigin() - center).squaredLength() - radius * radius;

        float delta = b * b - 4 * a * c;

        if (delta < 0) return false;
        
        float t1 = (-b - sqrt(delta)) / (2 * a);

        if (t1 > h.getT()) return false;
        if (t1 > tmin) {
            Vector3f normal = r.pointAtParameter(t1) - center;
            normal.normalize();
            h.set(t1, material, normal);
            return true;
        }

        float t2 = (-b + sqrt(delta)) / (2 * a);

        if (t2 > tmin && t2 < h.getT()) {
            Vector3f normal = r.pointAtParameter(t2) - center;
            normal.normalize();
            h.set(t1, material, normal);
            return true;
        }

        return false;
    }

protected:
    Vector3f center;
    float radius;
};


#endif
