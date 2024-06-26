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

    Hit(float _t, Material *m, const Vector3f &n, const Vector2f& _uv = Vector2f(0, 0), 
            const Vector3f& _tangent = Vector3f::ZERO, const Vector3f& _bitangent = Vector3f::ZERO) {
        t = _t;
        material = m;
        normal = n;
        uv = _uv;
        tangent = _tangent;
        bitangent = _bitangent;
    }

    Hit(const Hit &h) {
        t = h.t;
        material = h.material;
        normal = h.normal;
        uv = h.uv;
        tangent = h.tangent;
        bitangent = h.bitangent;
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

    const Vector3f &getTangent() const {
        return tangent;
    }

    const Vector3f &getBitangent() const {
        return bitangent;
    }

    void set(float _t, Material *m, const Vector3f &n, 
            const Vector2f& _uv = Vector2f(0, 0), 
            const Vector3f& _tangent = Vector3f::ZERO, 
            const Vector3f& _bitangent = Vector3f::ZERO) {
        t = _t;
        material = m;
        normal = n;
        uv = _uv;
        tangent = _tangent;
        bitangent = _bitangent;
    }

private:
    float t;
    Material *material;
    Vector3f normal;
    Vector3f tangent;
    Vector3f bitangent;
    Vector2f uv;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h) {
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
