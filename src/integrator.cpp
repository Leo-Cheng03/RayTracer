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

Vector3f Integrator::SampleL(const SceneParser& scene, const Ray& ray, Sampler& sampler, bool log, int depth) {
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
    if (primitives->intersect(ray, hit, 0.001f)) {
        // std::cout << "Hit at depth " << depth << std::endl;

        // std::cout << "Hit point: " << ray.getOrigin() + ray.getDirection() * hit.getT() << std::endl;
        // std::cout << "Hit normal: " << hit.getNormal() << std::endl;
        // std::cout << "Hit direction: " << -ray.getDirection() << std::endl;

        // If it does, get the surface normal and material properties
        Vector3f hitPoint = ray.pointAtParameter(hit.getT());
        Vector3f normal = hit.getNormal();
        Vector3f wo = -ray.getDirection();
        Vector3f wi;
        
        Vector3f wn = Vector3f::cross(wo, normal).normalized();
        Matrix3f normalToWorld = Matrix3f(
            Vector3f::cross(wn, normal).normalized(),
            normal,
            wn
        );
        Matrix3f worldToNormal = normalToWorld.inverse();

        Vector3f localWo = (worldToNormal * wo).normalized();

        Material* material = hit.getMaterial();

        // Direct lighting
        // Sample a light based on power heuristic
        Light* light = scene.getLight(
            (int)(sampler.Get1D() * scene.getNumLights())
        );
        LightSample ls;
        light->SampleLi(hitPoint, sampler.Get2D(), ls);
        // Create a new ray from the intersection point to the light source
        Ray shadowRay(hitPoint, ls.wi);
        // Check if the new ray intersects with any object in the scene
        Vector3f localWi = (worldToNormal * ls.wi).normalized();
        float tmax = ls.distance;

        if (log) {
            std::cout << "Hit at depth " << depth << std::endl;
            std::cout << "Hit point: " << hitPoint << std::endl;
            std::cout << "Hit normal: " << normal << std::endl;
            std::cout << "Hit direction: " << -wo << std::endl;
        }

        if (ls.pdf != 0 && !primitives->intersectP(shadowRay, 0.001f, tmax)) {
            // return {1, 1, 1};
            finalColor += ls.Li * std::abs(Vector3f::dot(ls.wi, normal)) / ls.pdf * material->f(localWo, localWi);
        }

        if (log) {
            std::cout << "Direct lighting: " << finalColor << std::endl;
        }
        
        // Indirect lighting
        BSDFSample bs = material->Sample_f(localWo, sampler);
        Vector3f sampleDir = (normalToWorld * bs.wi).normalized();
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

        Vector3f Li = SampleL(scene, sampleRay, sampler, log, depth + 1);
        float cosTheta = Vector3f::dot(normal, sampleDir);
        float absCosTheta = std::abs(cosTheta);
        // finalColor += bs.f * Li / bs.pdf;
        // finalColor += (sampleDir + Vector3f(1, 1, 1)) / 2;
        finalColor += bs.f * Li * (bs.isSpecular ? 1 : absCosTheta) / bs.pdf;
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