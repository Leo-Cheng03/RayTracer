#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;

class Hit {
public:

    // constructors
    Hit() {
        material = nullptr;
        t = 1e38;
    }

    Hit(float _t, Material *m, const Vector3f &n, const Vector2f& _uv = Vector2f(0, 0)) {
        t = _t;
        material = m;
        normal = n;
        uv = _uv;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        uv = h.uv;
    }

    // destructor
    ~Hit() = default;

    float getT() const {
        return t;
    }

    Material *getMaterial() const {
        return material;
    }

    const Vector3f &getNormal() const {
        return normal;
    }

    const Vector2f &getUV() const {
        return uv;
    }

    void set(float _t, Material *m, const Vector3f &n, 
            const Vector2f& _uv = Vector2f(0, 0)) {
        t = _t;
        material = m;
        normal = n;
        uv = _uv;
    }

private:
    float t;
    Material *material;
    Vector3f normal;
    Vector2f uv;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
