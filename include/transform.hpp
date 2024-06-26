#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point) {
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir) {
    return (mat * Vector4f(dir, 0)).xyz();
}

class Transform : public Object3D {
public:
    Transform() {}

    Transform(const Matrix4f &m, Object3D *obj) : o(obj) {
        transform = m.inverse();
    }

    ~Transform() {
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin) const {
        Vector3f trSource = transformPoint(transform, r.getOrigin());
        Vector3f trDirection = transformDirection(transform, r.getDirection());
        Ray tr(trSource, trDirection);
        bool inter = o->intersect(tr, h, tmin);
        if (inter) {
            Vector3f tangent = h.getTangent() == Vector3f::ZERO ? 
                Vector3f::ZERO : 
                transformDirection(transform, h.getTangent()).normalized();

            Vector3f bitangent = h.getBitangent() == Vector3f::ZERO ?
                Vector3f::ZERO :
                transformDirection(transform, h.getBitangent()).normalized();

            h.set(h.getT(), h.getMaterial(), 
                  transformDirection(transform.transposed(), h.getNormal()).normalized(), 
                  h.getUV(), tangent, bitangent
            );
        }
        return inter;
    }

    virtual bool intersectP(const Ray& ray, float tmin, float tmax) const {
        Vector3f trSource = transformPoint(transform, ray.getOrigin());
        Vector3f trDirection = transformDirection(transform, ray.getDirection());
        Ray tr(trSource, trDirection);
        return o->intersectP(tr, tmin, tmax);
    }

    Bound3f Bounds() const {
        return o->Bounds();
    }

protected:
    Object3D *o; //un-transformed object
    Matrix4f transform;
};

#endif //TRANSFORM_H
