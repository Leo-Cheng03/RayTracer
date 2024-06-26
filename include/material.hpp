#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "bsdf.hpp"
#include "texturesampler.hpp"
#include <iostream>

class Material {
public:
    virtual BSDFSample Sample_f(const Vector3f& wo, 
        const MaterialSampleContext& context, Sampler& sampler) = 0;
    virtual Vector3f f(const Vector3f& wo, const Vector3f& wi, 
        const MaterialSampleContext& context) = 0;
    
    virtual Matrix3f GetTangentToWorld(const MaterialSampleContext& context) {
        // std::cout << "tangent:   " << context.tangent << std::endl;
        // std::cout << "normal:    " << context.normal << std::endl;
        // std::cout << "bitangent: " << context.bitangent << std::endl;        
        return Matrix3f(
            context.tangent.normalized(),
            context.normal.normalized(),
            context.bitangent.normalized()
        );
    }

    virtual void PrintName() {
        std::cout << "Unnamed Material" << std::endl;
    }

    bool debug = false;
};

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial(const Vector3f& albedo) : 
        albedo(new Vector3fConstant(albedo)) {}
    
    DiffuseMaterial(const std::string& albedo) : 
        albedo(new Vector3fTexture(albedo)) {}

    DiffuseMaterial(Vector3fSampler* albedo, Vector3fSampler* normalMap = new Vector3fConstant(Vector3f(0.5, 0.5, 1.0))) : 
        albedo(albedo), normalMap(normalMap) {}

    ~DiffuseMaterial() {
        delete albedo;
    }

    BSDFSample Sample_f(const Vector3f& wo, const MaterialSampleContext& context, Sampler& sampler) override {
        BSDFSample sample;
        Vector3f wi = SampleCosineHemisphere(sampler.Get2D());
        if (wo.y() < 0) wi.y() = -wi.y();
        sample.wi = wi;
        sample.f = f(wo, wi, context);
        sample.pdf = CosineHemispherePDF(wi.y() > 0 ? wi.y() : -wi.y());
        return sample;
    }

    Vector3f f(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) override {
        if (wo.y() <= 0) return Vector3f::ZERO;
        return albedo->Sample(context) / M_PI;
    }

    Matrix3f GetTangentToWorld(const MaterialSampleContext& context) override {
        Matrix3f tangentToWorld = Material::GetTangentToWorld(context);
        Vector3f xzy = normalMap->Sample(context);
        Vector3f normal = {xzy.x(), xzy.z(), xzy.y()};
        normal = (normal * 2 - Vector3f(1, 1, 1)).normalized();
        normal = tangentToWorld * normal;
        
        Vector3f tangent;
        if (normal == Vector3f::UP) {
            tangent = Vector3f::RIGHT;
        } else {
            tangent = Vector3f::cross(Vector3f::UP, normal).normalized();
        }
        Vector3f bitangent = Vector3f::cross(normal, tangent).normalized();

        return Matrix3f(context.tangent, normal, context.bitangent);
    }

    void PrintName() override {
        std::cout << "Diffuse Material" << std::endl;
    }

private:
    Vector3fSampler* albedo;
    Vector3fSampler* normalMap;
};

class MetalicMaterial : public Material {
public:
    // MetalicMaterial(Vector3fSampler* albedo, float roughness) : 
    //     albedo(albedo), roughness(roughness) {}

    // Vector3f f(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) override {
    //     return Vector3f::ZERO;
    // }

    // BSDFSample Sample_f(const Vector3f& wo, const MaterialSampleContext& context, Sampler& sampler) override {
    //     BSDFSample sample;
    //     Vector2f u = sampler.Get2D();
    //     float theta = M_PI / 2 * roughness;
    //     float phi = 2 * M_PI * u.y();
    //     Vector3f wm = Vector3f(std::sin(theta) * std::cos(phi), std::cos(theta), std::sin(theta) * std::sin(phi)).normalized();

    //     Vector3f wi = Reflect(wo, wm);
        
    //     sample.f = albedo->Sample(context);
    //     sample.pdf = 1;
    //     sample.wi = wi;
    //     sample.isSpecular = true;
    //     return sample;
    // }

    MetalicMaterial(Vector3fSampler* albedo, float roughness) : 
        albedo(albedo), roughness(roughness), ggx(roughness, roughness) {}

    Vector3f f(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) override {
        if (ggx.EffectivelySmooth()) return Vector3f::ZERO;

        float cosTheta_o = AbsCosTheta(wo);
        float cosTheta_i = AbsCosTheta(wi);
        if (cosTheta_i == 0 || cosTheta_o == 0) return Vector3f::ZERO;
        Vector3f wm = wo + wi;

        if (wm == Vector3f::ZERO) return Vector3f::ZERO;

        wm = wm.y() > 0 ? wm.normalized() : -wm.normalized();

        return ggx.D(wm) * ggx.G(wo, wi) * albedo->Sample(context) / (4 * cosTheta_i * cosTheta_o);
    }

    BSDFSample Sample_f(const Vector3f& wo, const MaterialSampleContext& context, Sampler& sampler) override {
        if (ggx.EffectivelySmooth()) {
            BSDFSample sample;
            sample.wi = Reflect(wo, Vector3f::UP);
            sample.f = albedo->Sample(context);
            sample.pdf = 1;
            sample.isSpecular = true;
            return sample;
        }
        
        BSDFSample sample;
        Vector3f wm = ggx.Sample_wm(wo, sampler.Get2D()).normalized();
        Vector3f wi = Reflect(wo, wm);

        // std::cout << "wo: " << wo << std::endl;
        // std::cout << "wm: " << wm << std::endl;
        // std::cout << "wi: " << wi << std::endl;

        if (wi.y() * wo.y() <= 0) {
            sample.pdf = 0;
            sample.f = 0;
            sample.wi = Vector3f::ZERO;
            return sample;
        }

        float cosTheta_o = AbsCosTheta(wo);
        float cosTheta_i = AbsCosTheta(wi);

        sample.pdf = ggx.PDF(wo, wm) / (4 * AbsDot(wo, wm));
        sample.f = ggx.D(wm) * ggx.G(wo, wi) * albedo->Sample(context) / (4 * cosTheta_i * cosTheta_o);
        sample.wi = wi;
        sample.isSpecular = false;

        // std::cout << "f: " << sample.f << std::endl;
        // std::cout << "pdf: " << sample.pdf << std::endl;

        return sample;
    }

    float PDF(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) {
        Vector3f wm = wo + wi;
        if (wm == Vector3f::ZERO) return 0;
        wm = wm.y() > 0 ? wm.normalized() : -wm.normalized();
        return ggx.PDF(wo, wm) / (4 * AbsDot(wo, wm));
    }

    void PrintName() override {
        std::cout << "Metalic Material" << std::endl;
    }

private:
    Vector3fSampler* albedo;
    float roughness;
    GGXDistribution ggx;
};

class SpecularReflectionMaterial : public Material {
public:
    SpecularReflectionMaterial(const Vector3f& albedo) : 
        albedo(new Vector3fConstant(albedo)) {}

    SpecularReflectionMaterial(const std::string& albedo) : 
        albedo(new Vector3fTexture(albedo)) {}

    SpecularReflectionMaterial(Vector3fSampler* albedo) : 
        albedo(albedo) {}

    ~SpecularReflectionMaterial() {
        delete albedo;
    }

    Vector3f f(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) override {
        return Vector3f::ZERO;
    }

    BSDFSample Sample_f(const Vector3f& wo, const MaterialSampleContext& context, Sampler& sampler) override {
        BSDFSample sample;
        sample.wi = Reflect(wo, Vector3f::UP);
        sample.f = albedo->Sample(context);
        sample.pdf = 1;
        sample.isSpecular = true;
        return sample;
    }

    void PrintName() override {
        std::cout << "Specular Reflection Material" << std::endl;
    }

private:
    Vector3fSampler* albedo;
};

class SpecularTransmissionMaterial : public Material {
public:
    SpecularTransmissionMaterial(const Vector3f& albedo, float eta) : 
        albedo(new Vector3fConstant(albedo)), eta(eta) {}
    
    SpecularTransmissionMaterial(const std::string& albedo, float eta) : 
        albedo(new Vector3fTexture(albedo)), eta(eta) {}

    SpecularTransmissionMaterial(Vector3fSampler* albedo, float eta) : 
        albedo(albedo), eta(eta) {}

    ~SpecularTransmissionMaterial() {
        delete albedo;
    }

    Vector3f f(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) override {
        return Vector3f::ZERO;
    }

    BSDFSample Sample_f(const Vector3f& wo, const MaterialSampleContext& context, Sampler& sampler) override {
        BSDFSample result;
        float Fr = FresnelDieletric(wo.y(), eta);
        if (sampler.Get1D() < Fr) {
            result.wi = Reflect(wo, Vector3f(0, 1, 0));
            result.f = albedo->Sample(context) * Fr;
            result.pdf = Fr;
        } else {
            result.wi = Refract(wo, Vector3f(0, 1, 0), eta);
            result.f = albedo->Sample(context) * (1 - Fr);
            result.pdf = 1 - Fr;
            result.isTransmissive = true;
        }

        result.eta = eta;
        result.isSpecular = true;
        return result;
    }

    void PrintName() override {
        std::cout << "Specular Transmission Material" << std::endl;
    }

private:
    Vector3fSampler* albedo;
    float eta;
};

class MixedMaterial : public Material {
public:
    MixedMaterial(Material* m1, Material* m2, FloatSampler* ratio) : 
        material1(m1), material2(m2), ratio(ratio) {}

    ~MixedMaterial() {
        delete material1;
        delete material2;
        delete ratio;
    }

    Vector3f f(const Vector3f& wo, const Vector3f& wi, const MaterialSampleContext& context) override {
        return material1->f(wo, wi, context) * ratio->Sample(context) + 
            material2->f(wo, wi, context) * (1 - ratio->Sample(context));
    }

    BSDFSample Sample_f(const Vector3f& wo, const MaterialSampleContext& context, Sampler& sampler) override {
        if (sampler.Get1D() < ratio->Sample(context)) {
            return material1->Sample_f(wo, context, sampler);
        } else {
            return material2->Sample_f(wo, context, sampler);
        }
    }

    void PrintName() override {
        std::cout << "Mixed Material" << std::endl;
    }

private:
    Material* material1;
    Material* material2;
    FloatSampler* ratio;
};

#endif // MATERIAL_H
