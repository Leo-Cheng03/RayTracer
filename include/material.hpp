#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "bsdf.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {

    }

    virtual ~Material() {
        delete bsdf;
    }

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;
        
        Vector3f reflected = 2 * Vector3f::dot(dirToLight, hit.getNormal()) * hit.getNormal() - dirToLight;
        Vector3f diffuse = diffuseColor * std::max(Vector3f::dot(hit.getNormal(), dirToLight), 0.0f);
        Vector3f specular = specularColor * std::pow(std::max(Vector3f::dot(-ray.getDirection(), reflected), 0.0f), shininess);
        shaded += lightColor * (diffuse + specular);

        return shaded;
    }

    BSDFSample Sample_f(const Vector3f& wo) {
        return bsdf->Sample_f(wo);
    }

    Vector3f f(const Vector3f& wo, const Vector3f& wi) {
        return bsdf->f(wo, wi);
    }

    BSDF* getBSDF() {
        return bsdf;
    }

    void setBSDF(BSDF* b) {
        bsdf = b;
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
    BSDF* bsdf;
};

#endif // MATERIAL_H
