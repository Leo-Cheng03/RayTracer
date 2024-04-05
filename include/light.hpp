#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>
#include "object3d.hpp"

struct LightSample {
    Vector3f Li;
    Vector3f wi;
    float pdf;
};

class Light {
public:
    Light() = default;

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const = 0;

    virtual bool SampleLi(const Vector3f &p, LightSample &ls) const = 0;

    // Find emitted radiance of area light.
    virtual Vector3f L(const Vector3f &p, const Vector3f &n, const Vector2f &uv, const Vector3f &w) {
        return Vector3f::ZERO;
    }
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c) {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight() override = default;

    ///@param p unsed in this function
    ///@param distanceToLight not well defined because it's not a point light
    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = -direction;
        col = color;
    }

    bool SampleLi(const Vector3f &p, LightSample &ls) const override {
        ls.Li = color;
        ls.wi = -direction;
        ls.pdf = 1;
        return true;
    }

private:

    Vector3f direction;
    Vector3f color;

};

class PointLight : public Light {
public:
    PointLight() = delete;

    PointLight(const Vector3f &p, const Vector3f &c) {
        position = p;
        color = c;
    }

    ~PointLight() override = default;

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        dir = dir / dir.length();
        col = color;
    }

    bool SampleLi(const Vector3f &p, LightSample &ls) const override {
        ls.Li = (position - p).squaredLength() * color;
        ls.wi = ls.Li.normalized();
        ls.pdf = 1;
        return true;
    }

private:

    Vector3f position;
    Vector3f color;

};

#endif // LIGHT_H
