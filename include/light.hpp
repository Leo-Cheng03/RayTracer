#ifndef LIGHT_H
#define LIGHT_H

#include "vecmath.h"
#include "object3d.hpp"

inline float ClampZero(float x) {
    return x < 0 ? 0 : x ;
}

struct LightSample {
    Vector3f Li;
    Vector3f wi;
    float distance;
    float pdf;
};

class Light {
public:
    Light(float s = 0.0f) : scale(s) {}

    virtual ~Light() = default;

    virtual void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const = 0;

    virtual bool SampleLi(const Vector3f &p, const Vector2f& uv, LightSample &ls) const = 0;

    // Find emitted radiance of area light.
    virtual Vector3f L(const Vector3f &p, const Vector3f &n, const Vector2f &uv, const Vector3f &w) {
        return Vector3f::ZERO;
    }

protected:
    float scale;
};


class DirectionalLight : public Light {
public:
    DirectionalLight() = delete;

    DirectionalLight(const Vector3f &d, const Vector3f &c, float s = 0.0f) : Light(s) {
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

    bool SampleLi(const Vector3f &p, const Vector2f& uv, LightSample &ls) const override {
        ls.Li = color * scale;
        ls.wi = -direction;
        ls.distance = INFINITY;
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

    PointLight(const Vector3f &p, const Vector3f &c, float s = 0.0f) : Light(s) {
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

    bool SampleLi(const Vector3f &p, const Vector2f& uv, LightSample &ls) const override {
        ls.Li = color * scale / (position - p).squaredLength();
        ls.distance = (position - p).length();
        ls.wi = (position - p) / ls.distance;
        ls.pdf = 1;
        return true;
    }

private:
    Vector3f position;
    Vector3f color;
};

class AreaLight : public Light {
public:
    AreaLight() = delete;

    AreaLight(const Vector3f &p, const Vector3f& dir, const Vector3f& up, const Vector2f size, const Vector3f &c, float s = 0.0f) : Light(s) {
        this->position = p;
        this->color = c;
        this->direction = dir.normalized();
        this->up = (up - Vector3f::dot(up, direction) * direction).normalized();
        this->right = Vector3f::cross(this->direction, this->up);
        this->size = size;
        this->pdf = 1 / (size.x() * size.y());
    }

    ~AreaLight() override = default;

    void getIllumination(const Vector3f &p, Vector3f &dir, Vector3f &col) const override {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        dir = (position - p);
        dir = dir / dir.length();
        col = color;
    }

    bool SampleLi(const Vector3f &p, const Vector2f& uv, LightSample &ls) const override {
        Vector3f samplePoint = position + (uv.x() - 0.5) * size.x() * right + (uv.y() - 0.5) * size.y() * up;

        ls.distance = (samplePoint - p).length();
        ls.wi = (samplePoint - p) / ls.distance;
        ls.Li = color * scale * ClampZero(Vector3f::dot(-ls.wi, direction)) / (samplePoint - p).squaredLength();
        ls.pdf = pdf;
        return true;
    }

private:
    Vector3f position;
    Vector3f direction;
    Vector3f up;
    Vector3f right;
    Vector2f size;
    Vector3f color;
    float pdf;
};

#endif // LIGHT_H
