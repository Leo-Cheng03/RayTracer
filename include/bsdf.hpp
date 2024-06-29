#ifndef BSDF_HPP
#define BSDF_HPP

#include <vecmath.h>
#include <sampling.hpp>
#include <sampler.hpp>

static float _Lerp(float t, float a, float b) {
    return a + t * (b - a);
}

inline float CosTheta(const Vector3f& w) {
    return w.y();
}

inline float CosSquaredTheta(const Vector3f& w) {
    return w.y() * w.y();
}

inline float AbsCosTheta(const Vector3f& w) {
    return std::abs(w.y());
}

inline float AbsDot(const Vector3f& a, const Vector3f& b) {
    return std::abs(Vector3f::dot(a, b));
}

inline float SinSquaredTheta(const Vector3f& w) {
    return 1 - CosSquaredTheta(w);
}

inline float SinTheta(const Vector3f& w) {
    return std::sqrt(SinSquaredTheta(w));
}

inline float CosPhi(const Vector3f& w) {
    float sinTheta = SinTheta(w);
    return sinTheta == 0 ? 1 : std::clamp(w.x() / sinTheta, -1.0f, 1.0f);
}

inline float SinPhi(const Vector3f& w) {
    float sinTheta = SinTheta(w);
    return sinTheta == 0 ? 0 : std::clamp(w.z() / sinTheta, -1.0f, 1.0f);
}

inline float TanTheta(const Vector3f& w) {
    return SinTheta(w) / CosTheta(w);
}

inline float TanSquaredTheta(const Vector3f& w) {
    return SinSquaredTheta(w) / CosSquaredTheta(w);
}

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

class GGXDistribution {
public:
    GGXDistribution(float alphaX, float alphaZ) : 
        alphaX(alphaX), alphaZ(alphaZ) {}
    
    Vector3f Sample_wm(const Vector3f& w, const Vector2f& u) const {
        Vector3f wh = Vector3f(alphaX * w.x(), w.y(), alphaZ * w.z()).normalized();
        if (wh.y() < 0) wh = -wh;

        Vector3f T1 = (wh.y() < 0.9999f) ? Vector3f::cross(Vector3f::UP, wh).normalized() : 
                                           Vector3f::RIGHT;
        Vector3f T2 = Vector3f::cross(wh, T1);

        Vector2f p = SampleUniformDiskPolar(u);

        float h = std::sqrt(1 - p.x() * p.x());
        p.y() = _Lerp((1 + wh.y()) / 2, h, p.y());

        float pz = std::sqrt(std::max<float>(0, 1 - p.lengthSquared()));
        Vector3f nh = p.x() * T1 + p.y() * T2 + pz * wh;
        return Vector3f(alphaX * nh.x(), std::max<float>(1e-6f, nh.y()), alphaZ * nh.z()).normalized();
    }

    float D(const Vector3f& wm, bool debug = false) const {
        float tanSquaredTheta = TanSquaredTheta(wm);
        if (std::isinf(tanSquaredTheta)) return 0;
        float cos4Theta = CosSquaredTheta(wm) * CosSquaredTheta(wm);
        float e = tanSquaredTheta * ((CosPhi(wm) / alphaX) * (CosPhi(wm) / alphaX) + 
                                     (SinPhi(wm) / alphaZ) * (SinPhi(wm) / alphaZ));
        
        if (debug) {
            std::cout << "wm: " << wm << std::endl;
            std::cout << "tan^2(theta): " << tanSquaredTheta << std::endl;
            std::cout << "cos^4(theta): " << cos4Theta << std::endl;
            std::cout << "e: " << e << std::endl;
        }

        return 1 / (M_PI * alphaX * alphaZ * cos4Theta * (1 + e) * (1 + e));
    }

    float D(const Vector3f& w, const Vector3f& wm) const {
        return G1(w) / AbsCosTheta(w) * D(wm) * AbsDot(w, wm);
    }

    float PDF(const Vector3f& w, const Vector3f& wm) const {
        return D(w, wm);
    }

    bool EffectivelySmooth() const {
        return std::max(alphaX, alphaZ) < 1e-3;
    }

    float Lambda(const Vector3f& w) const {
        float tanSquaredTheta = TanSquaredTheta(w);
        if (std::isinf(tanSquaredTheta)) return 0;
        float alphaSquared = CosPhi(w) * CosPhi(w) * alphaX * alphaX + 
                       SinPhi(w) * SinPhi(w) * alphaZ * alphaZ;
        return (std::sqrt(1 + alphaSquared * tanSquaredTheta) - 1) / 2;
    }

    float G1(const Vector3f& w) const {
        return 1 / (1 + Lambda(w));
    }

    float G(const Vector3f& wo, const Vector3f& wi) const {
        return 1 / (1 + Lambda(wo) + Lambda(wi));
    }

private:
    float alphaX, alphaZ;
};

// class BSDF {
// public:
//     BSDF() {}
//     ~BSDF() = default;

//     virtual Vector3f f(const Vector3f& wo, const Vector3f& wi) = 0;
//     virtual BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) = 0;
//     virtual bool IsTransparent() { return false; }

// protected:
// };

// enum BSDFType {
//     DIFFUSE,
//     SPECULAR_DIELETRIC,
//     SPECULAR_REFLECTANCE,
// };

// class DiffuseBRDF : public BSDF {
// public:
//     DiffuseBRDF(const Vector3f& color) : albedo(color) {}

//     Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
//         if (wo.y() <= 0) return Vector3f::ZERO;
//         return albedo / M_PI;
//     }

//     BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) override {
//         BSDFSample sample;
//         Vector3f wi = SampleCosineHemisphere(sampler.Get2D());
//         if (wo.y() < 0) wi.y() = -wi.y();
//         sample.wi = wi;
//         sample.f = f(wo, wi);
//         sample.pdf = CosineHemispherePDF(wi.y() > 0 ? wi.y() : -wi.y());
//         return sample;
//     }

// private:
//     Vector3f albedo;
// };

// class SpecularReflectanceBSDF : public BSDF {
// public:
//     SpecularReflectanceBSDF(const Vector3f& color) : albedo(color) {}

//     Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
//         return Vector3f::ZERO;
//     }

//     BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) override {
//         BSDFSample sample;
//         sample.wi = Reflect(wo, Vector3f::UP);
//         sample.f = albedo;
//         sample.pdf = 1;
//         sample.isSpecular = true;
//         return sample;
//     }

// private:
//     Vector3f albedo;
// }; 

// class SpecularBSDF : public BSDF {
// public:
//     SpecularBSDF(const Vector3f& color, float eta) : albedo(color), eta(eta) {}

//     Vector3f f(const Vector3f& wo, const Vector3f& wi) override {
//         return Vector3f::ZERO;
//     }

//     BSDFSample Sample_f(const Vector3f& wo, Sampler& sampler) override {
//         BSDFSample result;
//         float Fr = FresnelDieletric(wo.y(), eta);
//         if (sampler.Get1D() < Fr) {
//             result.wi = Reflect(wo, Vector3f(0, 1, 0));
//             result.f = albedo * Fr;
//             result.pdf = Fr;
//         } else {
//             result.wi = Refract(wo, Vector3f(0, 1, 0), eta);
//             result.f = albedo * (1 - Fr);
//             result.pdf = 1 - Fr;
//             result.isTransmissive = true;
//         }
//         // result.wi = -wo;
//         // result.wi = Refract(wo, Vector3f::UP, eta);
//         // result.f = albedo;
//         // result.pdf = 1;
//         // result.isTransmissive = true;
//         result.eta = eta;
//         result.isSpecular = true;
//         return result;
//     }

//     bool IsTransparent() override { return true; }

// private:
//     Vector3f albedo;
//     float eta;
// };

#endif