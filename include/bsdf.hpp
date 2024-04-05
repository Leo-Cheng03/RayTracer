#ifndef BSDF_HPP
#define BSDF_HPP

#include <vecmath.h>

struct BSDFSample {
    Vector3f f;
    Vector3f wi;
    float pdf;
    float eta;
};

class BSDF {
public:
    BSDF() {}
    ~BSDF() = default;

    virtual Vector3f f(const Vector3f& wo, const Vector3f& wi) = 0;
    virtual BSDFSample Sample_f(const Vector3f& wo) = 0;

protected:
};

class DiffuseBRDF : public BSDF {
public:
    DiffuseBRDF(const Vector3f& color) : albedo(color) {}

    Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
        if (wo.y() <= 0) return Vector3f::ZERO;
        return albedo / M_PI;
    }

    BSDFSample Sample_f(const Vector3f& wo) override {
        BSDFSample sample;
        Vector3f randomDir = Vector3f(rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f).normalized();
        sample.wi = randomDir;
        sample.f = f(wo, sample.wi);
        sample.pdf = Vector3f::dot(sample.wi, Vector3f::UP) / M_PI;
        return sample;
    }

private:
    Vector3f albedo;
};

#endif