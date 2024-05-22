#ifndef BOUND_HPP
#define BOUND_HPP

#include <vecmath.h>
#include <cmath>
#include <cfloat>

class Bound3f {
private:
    Vector3f pMin, pMax;

public:
    Bound3f() {
        pMin = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
        pMax = Vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

    Bound3f(const Vector3f& p1, const Vector3f& p2) {
        pMin = Vector3f(
            std::min(p1.x(), p2.x()),
            std::min(p1.y(), p2.y()),
            std::min(p1.z(), p2.z())
        );
        pMax = Vector3f(
            std::max(p1.x(), p2.x()),
            std::max(p1.y(), p2.y()),
            std::max(p1.z(), p2.z())
        );
    }

    explicit Bound3f(const Vector3f& p) : pMin(p), pMax(p) {}

    Vector3f operator[](int i) const {
        return i == 0 ? pMin : pMax;
    }

    Vector3f& operator[](int i) {
        return i == 0 ? pMin : pMax;
    }

    Vector3f Corner(int corner) const {
        return Vector3f(
            (*this)[(corner & 1)].x(),
            (*this)[(corner & 2) ? 1 : 0].y(),
            (*this)[(corner & 4) ? 1 : 0].z()
        );
    }

    Vector3f Diagonal() const {
        return pMax - pMin;
    }

    float SurfaceArea() const {
        Vector3f d = Diagonal();
        return 2 * (d.x() * d.y() + d.x() * d.z() + d.y() * d.z());
    }

    float Volume() const {
        Vector3f d = Diagonal();
        return d.x() * d.y() * d.z();
    }

    int MaxDimension() const {
        Vector3f d = Diagonal();
        if (d.x() > d.y() && d.x() > d.z()) {
            return 0;
        }
        else if (d.y() > d.z()) {
            return 1;
        }
        else {
            return 2;
        }
    }

    Vector3f Lerp(float tx, float ty, float tz) const {
        return Vector3f(
            tx * pMin.x() + (1 - tx) * pMax.x(),
            ty * pMin.y() + (1 - ty) * pMax.y(),
            tz * pMin.z() + (1 - tz) * pMax.z()
        );
    }

    Vector3f Offset(const Vector3f& p) const {
        Vector3f o = p - pMin;
        if (pMax.x() > pMin.x()) o.x() /= pMax.x() - pMin.x();
        if (pMax.y() > pMin.y()) o.y() /= pMax.y() - pMin.y();
        if (pMax.z() > pMin.z()) o.z() /= pMax.z() - pMin.z();
        return o;
    }

    void BoundingSphere(Vector3f& center, float& radius) const {
        center = (pMin + pMax) / 2;
        radius = Inside(center, *this) ? (center - pMax).length() : 0;
    }

    bool IsEmpty() const {
        return pMin.x() >= pMax.x() || pMin.y() >= pMax.y() || pMin.z() >= pMax.z();
    }

    bool IsDegenerate() const {
        return pMin.x() > pMax.x() || pMin.y() > pMax.y() || pMin.z() > pMax.z();
    }

    // Inline functions

    static bool Inside(const Vector3f& p, const Bound3f& b) {
        return p.x() >= b[0].x() && p.x() <= b[1].x() &&
            p.y() >= b[0].y() && p.y() <= b[1].y() &&
            p.z() >= b[0].z() && p.z() <= b[1].z();
    }

    static Bound3f Union(const Bound3f& b, const Vector3f& p) {
        Bound3f ret;
        ret.pMin = Vector3f(
            std::min(b.pMin.x(), p.x()),
            std::min(b.pMin.y(), p.y()),
            std::min(b.pMin.z(), p.z())
        );
        ret.pMax = Vector3f(
            std::max(b.pMax.x(), p.x()),
            std::max(b.pMax.y(), p.y()),
            std::max(b.pMax.z(), p.z())
        );
        return ret;
    }

    static Bound3f Union(const Bound3f& b1, const Bound3f& b2) {
        Bound3f ret;
        ret.pMin = Vector3f(
            std::min(b1.pMin.x(), b2.pMin.x()),
            std::min(b1.pMin.y(), b2.pMin.y()),
            std::min(b1.pMin.z(), b2.pMin.z())
        );
        ret.pMax = Vector3f(
            std::max(b1.pMax.x(), b2.pMax.x()),
            std::max(b1.pMax.y(), b2.pMax.y()),
            std::max(b1.pMax.z(), b2.pMax.z())
        );
        return ret;
    }

    static Bound3f Intersect(const Bound3f& b1, const Bound3f& b2) {
        Bound3f ret;
        ret.pMin = Vector3f(
            std::max(b1.pMin.x(), b2.pMin.x()),
            std::max(b1.pMin.y(), b2.pMin.y()),
            std::max(b1.pMin.z(), b2.pMin.z())
        );
        ret.pMax = Vector3f(
            std::min(b1.pMax.x(), b2.pMax.x()),
            std::min(b1.pMax.y(), b2.pMax.y()),
            std::min(b1.pMax.z(), b2.pMax.z())
        );
        return ret;
    }

    static Bound3f Expand(const Bound3f& b, float delta) {
        return Bound3f(b.pMin - Vector3f(delta, delta, delta), b.pMax + Vector3f(delta, delta, delta));
    }
};



#endif