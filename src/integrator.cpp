#include "integrator.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "light.hpp"
#include "sampler.hpp"
#include "sampling.hpp"

#include <stdio.h>

bool russianRoulette(float p) {
    return GetUniform1D() < p;
}

Vector3f randomDirection() {
    float abs_x = (float)rand() / RAND_MAX;
    float abs_y = (float)rand() / RAND_MAX;
    float abs_z = (float)rand() / RAND_MAX;
    float x = rand() % 2 == 0 ? abs_x : -abs_x;
    float y = rand() % 2 == 0 ? abs_y : -abs_y;
    float z = rand() % 2 == 0 ? abs_z : -abs_z;
    return Vector3f(x, y, z).normalized();
}

Vector3f Integrator::SampleL(const SceneParser& scene, const Ray& ray, Sampler& sampler, bool* hitLight, bool log, int depth) {
    if (depth > 10) return Vector3f::ZERO;

    Group* primitives = scene.getGroup();
    Hit hit;
    Vector3f finalColor = Vector3f::ZERO;

    // Russian roulette
    float survivalRate = 1.0f;
    if (depth > 4) {
        survivalRate = 0.2f;
        if (russianRoulette(1 - survivalRate)) {
            return Vector3f::ZERO;
        }
    }

    // Check if the ray intersects with any object in the scene
    if (primitives->intersect(ray, hit, 0.005f)) {
        // std::cout << "Hit at depth " << depth << std::endl;

        // std::cout << "Hit point: " << ray.getOrigin() + ray.getDirection() * hit.getT() << std::endl;
        // std::cout << "Hit normal: " << hit.getNormal() << std::endl;
        // std::cout << "Hit direction: " << -ray.getDirection() << std::endl;

        // If it does, get the surface normal and material properties

        // return hit.getBitangent() / 2 + Vector3f(0.5, 0.5, 0.5);

        if (log) {
            std::cout << "Hit at depth " << depth << std::endl;
        }

        Vector3f hitPoint = ray.pointAtParameter(hit.getT());
        Vector3f normal = hit.getNormal();
        Vector3f wo = -ray.getDirection();
        Vector3f wi;
        MaterialSampleContext context(hit);

        if (log) {
            std::cout << "Hit info ok" << std::endl;
            std::cout << "normal: " << context.normal << std::endl;
            std::cout << "tangent: " << context.tangent << std::endl;
            std::cout << "bitangent: " << context.bitangent << std::endl;
        }
        
        Material* material = hit.getMaterial();

        if (material->IsLightSource()) {
            // std::cout << "emission: " << material->GetEmission() << std::endl;
            if (hitLight) *hitLight = true;
            return material->GetEmission() * Vector3f(0.5, 0.5, 0.5);
        }

        if (log) {
            std::cout << "Material ok" << std::endl;
            material->PrintName();
            material->debug = true;
        }

        Matrix3f tangentToWorld = material->GetTangentToWorld(context);

        if (log) {
            material->debug = false;
            std::cout << "tangentToWorld: " << std::endl;
            std::cout << tangentToWorld(0, 0) << " " << tangentToWorld(0, 1) << " " << tangentToWorld(0, 2) << std::endl;
            std::cout << tangentToWorld(1, 0) << " " << tangentToWorld(1, 1) << " " << tangentToWorld(1, 2) << std::endl;
            std::cout << tangentToWorld(2, 0) << " " << tangentToWorld(2, 1) << " " << tangentToWorld(2, 2) << std::endl;
            std::cout << std::endl;
        }

        Matrix3f worldToTangent = tangentToWorld.inverse();

        if (log) {
            std::cout << "Transform matrix ok" << std::endl;
        }

        Vector3f localWo = (worldToTangent * wo).normalized();

        // Direct lighting
        // Sample a light based on power heuristic
        int lightIndex = (int)(sampler.Get1D() * scene.getNumLights());
        lightIndex = std::min(lightIndex, scene.getNumLights() - 1);
        Light* light = scene.getLight(lightIndex);

        // if (log) { std::cout << "light ok: " << light << ", s = " << s << ", light num = " << lightNum << ", light index = " << lightIndex << std::endl; }

        LightSample ls;
        light->SampleLi(hitPoint, sampler.Get2D(), ls);

        // if (log) { std::cout << "light sampling ok" << std::endl; }

        // Create a new ray from the intersection point to the light source
        Ray shadowRay(hitPoint, ls.wi);
        // Check if the new ray intersects with any object in the scene
        Vector3f localWi = (worldToTangent * ls.wi).normalized();
        float tmax = ls.distance;

        // if (log) { std::cout << "direct wi ok" << std::endl; }

        if (log) {
            std::cout << "Hit point: " << hitPoint << std::endl;
            std::cout << "Hit normal: " << normal << std::endl;
            std::cout << "Hit direction: " << -wo << std::endl;
        }

        if (ls.pdf != 0 && (!primitives->intersectP(shadowRay, 0.001f, tmax, light->GetObject()))) {
            if (log) {
                std::cout << "tangent: " << context.tangent << std::endl;
                std::cout << "bitangent: " << context.bitangent << std::endl;

                std::cout << "Tangent to world: " << std::endl;
                std::cout << tangentToWorld(0, 0) << " " << tangentToWorld(0, 1) << " " << tangentToWorld(0, 2) << std::endl;
                std::cout << tangentToWorld(1, 0) << " " << tangentToWorld(1, 1) << " " << tangentToWorld(1, 2) << std::endl;
                std::cout << tangentToWorld(2, 0) << " " << tangentToWorld(2, 1) << " " << tangentToWorld(2, 2) << std::endl;

                material->PrintName();
                material->debug = true;
                std::cout << "localWo: " << localWo << std::endl;
                std::cout << "localWi: " << localWi << std::endl;
                std::cout << "f: " << material->f(localWo, localWi, context) << std::endl;
                std::cout << "pdf: " << ls.pdf << std::endl;
                material->debug = false;
            }
            finalColor += ls.Li * std::abs(Vector3f::dot(ls.wi, normal)) / ls.pdf * material->f(localWo, localWi, context);
        }

        if (log) {
            std::cout << "Direct lighting: " << finalColor << std::endl;
        }
        
        // Indirect lighting
        BSDFSample bs = material->Sample_f(localWo, context, sampler);
        if (bs.wi == Vector3f::ZERO) {
            // finalColor = Vector3f(0, 1, 0);
            return finalColor;
        }

        // if (bs.isSpecular) {
        //     Vector3f sampleDir = (tangentToWorld * bs.wi).normalized();
        //     return (sampleDir + Vector3f(1, 1, 1)) / 2;
        // }

        Vector3f sampleDir = (tangentToWorld * bs.wi).normalized();
        Ray sampleRay(hitPoint, sampleDir);

        if (log) {
            if (bs.isTransmissive) {
                std::cout << "incident angle (normal space): " << std::acos(std::abs(Vector3f::dot(localWo, Vector3f(0, 1, 0)))) * 180 / M_PI << std::endl;
                std::cout << "incident angle (world space): " << std::acos(std::abs(Vector3f::dot(wo, normal))) * 180 / M_PI << std::endl;
                std::cout << "transmissive angle: " << std::acos(std::abs(Vector3f::dot(sampleDir, normal))) * 180 / M_PI << std::endl;
                float sinTheta_i_squared = 1 - std::pow(Vector3f::dot(wo, normal), 2);
                float sinTheta_t_squared = 1 - std::pow(Vector3f::dot(sampleDir, normal), 2);
                std::cout << "calculated eta: " << std::sqrt(sinTheta_i_squared / sinTheta_t_squared) << std::endl;
            }
            std::cout << "Sample ray: " << sampleRay << std::endl;
        }

        bool isLightSource = false;
        Vector3f Li = SampleL(scene, sampleRay, sampler, &isLightSource, log, depth + 1);
        if (isLightSource && !bs.isSpecular) {
            return finalColor;
            // finalColor *= 0.5f;
            // Li = bs.isSpecular ? Li : Li / 2;
        }
        float cosTheta = Vector3f::dot(normal, sampleDir);
        float absCosTheta = std::abs(cosTheta);
        // finalColor += bs.f * Li / bs.pdf;
        // finalColor += (sampleDir + Vector3f(1, 1, 1)) / 2;
        finalColor += bs.f * Li * (bs.isSpecular ? 1 : absCosTheta) / bs.pdf;
        // finalColor = Vector3f(1, 0, 0);
        // The cosine term is cancelled out when calculating the pdf on a specular surface
        // https://computergraphics.stackexchange.com/questions/13250/cosine-term-in-rendering-equation
    }
    else {
        // If it doesn't, account for the background color
        finalColor += scene.getBackgroundColor();

        if (log) {
            std::cout << "To the void" << std::endl;
        }
    }

    // Russian roulette
    finalColor /= survivalRate;

    return finalColor;
}