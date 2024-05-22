#ifndef BSDF_HPP
#define BSDF_HPP

#include <vecmath.h>
#include <sampling.hpp>
#include <sampler.hpp>

inline Vector3f Reflect(const Vector3f& wo, const Vector3f& normal) {
    return -wo + 2 * Vector3f::dot(wo, normal) * normal;
}

inline Vector3f Refract(const Vector3f& wi, Vector3f normal, float eta) {
    float cosTheta_i = Vector3f::dot(wi, normal);
    if (cosTheta_i < 0) {
        cosTheta_i = -cosTheta_i;
        normal = -normal;
        eta = 1 / eta;
    }

    float sinTheta_i_squared = 1 - cosTheta_i * cosTheta_i;
    float sinTheta_t_squared = sinTheta_i_squared / (eta * eta);;
    if (sinTheta_t_squared > 1) {
        return Reflect(wi, normal);
    }
    float cosTheta_t = std::sqrt(1 - sinTheta_t_squared);
    return -wi / eta + (cosTheta_i / eta - cosTheta_t) * normal;
}

inline float FresnelDieletric(float cosTheta_i, float eta) {
    cosTheta_i = cosTheta_i > 1 ? 1 : (cosTheta_i < -1 ? -1 : cosTheta_i);
    if (cosTheta_i < 0) {
        eta = 1 / eta;
        cosTheta_i = -cosTheta_i;
    }

    float sin2Theta_i = 1 - cosTheta_i * cosTheta_i;
    float sin2Theta_t = sin2Theta_i / (eta * eta);
    if (sin2Theta_t >= 1) return 1.0f;
    float cosTheta_t = sqrt(1 - sin2Theta_t);

    float r_parl = (eta * cosTheta_i - cosTheta_t) / (eta * cosTheta_i + cosTheta_t);
    float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
    return (r_parl * r_parl + r_perp * r_perp) / 2;
}

struct BSDFSample {
    Vector3f f;
    Vector3f wi;
    float pdf;
    float eta;
    bool isTransmissive = false;
    bool isSpecular = false;
};

class BSDF {
public:
    BSDF() {}
    ~BSDF() = default;

    virtual Vector3f f(const Vector3f& wo, const Vector3f& wi) = 0;
    virtual BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) = 0;
    virtual bool IsTransparent() { return false; }

protected:
};

enum BSDFType {
    DIFFUSE,
    SPECULAR_DIELETRIC,
    SPECULAR_REFLECTANCE,
};

class DiffuseBRDF : public BSDF {
public:
    DiffuseBRDF(const Vector3f& color) : albedo(color) {}

    Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
        if (wo.y() <= 0) return Vector3f::ZERO;
        return albedo / M_PI;
    }

    BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) override {
        BSDFSample sample;
        Vector3f wi = SampleCosineHemisphere(sampler.Get2D());
        if (wo.y() < 0) wi.y() = -wi.y();
        sample.wi = wi;
        sample.f = f(wo, wi);
        sample.pdf = CosineHemispherePDF(wi.y() > 0 ? wi.y() : -wi.y());
        return sample;
    }

private:
    Vector3f albedo;
};

class SpecularReflectanceBSDF : public BSDF {
public:
    SpecularReflectanceBSDF(const Vector3f& color) : albedo(color) {}

    Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
        return Vector3f::ZERO;
    }

    BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) override {
        BSDFSample sample;
        sample.wi = Reflect(wo, Vector3f::UP);
        sample.f = albedo;
        sample.pdf = 1;
        sample.isSpecular = true;
        return sample;
    }

private:
    Vector3f albedo;
}; 

class SpecularBSDF : public BSDF {
public:
    SpecularBSDF(const Vector3f& color, float eta) : albedo(color), eta(eta) {}

    Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
        return Vector3f::ZERO;
    }

    BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) override {
        BSDFSample result;
        float Fr = FresnelDieletric(wo.y(), eta);
        if (sampler.Get1D() < Fr) {
            result.wi = Reflect(wo, Vector3f(0, 1, 0));
            result.f = albedo * Fr;
            result.pdf = Fr;
        } else {
            result.wi = Refract(wo, Vector3f(0, 1, 0), eta);
            result.f = albedo * (1 - Fr);
            result.pdf = 1 - Fr;
            result.isTransmissive = true;
        }
        // result.wi = -wo;
        // result.wi = Refract(wo, Vector3f::UP, eta);
        // result.f = albedo;
        // result.pdf = 1;
        // result.isTransmissive = true;
        result.eta = eta;
        result.isSpecular = true;
        return result;
    }

    bool IsTransparent() override { return true; }

private:
    Vector3f albedo;
    float eta;
};

#endif