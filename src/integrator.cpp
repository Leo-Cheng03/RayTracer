#include "integrator.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "light.hpp"
#include "sampler.hpp"

float russianRoulette(float p) {
    return (float)rand() / RAND_MAX < p;
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

Vector3f Integrator::SampleL(const SceneParser& scene, const Ray& ray, Sampler& sampler, int depth) {
    if (depth > 10) return Vector3f::ZERO;

    Group* primitives = scene.getGroup();
    Hit hit;
    Vector3f finalColor = Vector3f::ZERO;

    // Russian roulette
    float survivalRate = 1.0f;
    if (depth > 3) {
        survivalRate = 0.2f;
        if (russianRoulette(1 - survivalRate)) {
            return Vector3f::ZERO;
        }
    }

    // Check if the ray intersects with any object in the scene
    if (primitives->intersect(ray, hit, 0.001f)) {
        // If it does, get the surface normal and material properties
        Vector3f hitPoint = ray.pointAtParameter(hit.getT());
        Vector3f normal = hit.getNormal();
        Vector3f wo = -ray.getDirection();
        Vector3f wi;
        
        Vector3f wn = Vector3f::cross(wo, normal);
        Matrix3f normalToWorld = Matrix3f(
            Vector3f::cross(wn, normal),
            normal,
            wn
        );
        Matrix3f worldToNormal = normalToWorld.inverse();

        Vector3f localWo = worldToNormal * wo;

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
        Vector3f localWi = worldToNormal * ls.wi;
        float tmax = ls.distance;
        if (ls.pdf != 0 && !primitives->intersectP(shadowRay, 0.001f, tmax)) {
            finalColor += ls.Li * (Vector3f::dot(ls.wi, normal)) / ls.pdf * material->f(localWo, localWi);
        }
        
        // Indirect lighting
        Vector3f sampleDir = cosineSampleHemisphere(sampler.Get2D());
        Vector3f localSampleDir = worldToNormal * sampleDir;
        Ray sampleRay(hitPoint, sampleDir);
        finalColor += material->f(localWo, localSampleDir) * SampleL(scene, sampleRay, sampler, depth + 1);
    }
    else {
        // If it doesn't, account for the background color
        finalColor += scene.getBackgroundColor();
    }

    // Russian roulette
    finalColor /= survivalRate;

    return finalColor;
}