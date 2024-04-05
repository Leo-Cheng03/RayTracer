#include "integrator.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "light.hpp"

float russianRoulette(float p) {
    return (float)rand() / RAND_MAX < p;
}

Vector3f Integrator::SampleL(const SceneParser& scene, const Ray& ray, int depth) {
    if (depth > 10) return Vector3f::ZERO;

    Group* primitives = scene.getGroup();
    Hit hit;
    Vector3f finalColor = Vector3f::ZERO;

    // Russian roulette
    // std::cout << "hi" << std::endl;
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
        Light* light = scene.getLight(rand() % scene.getNumLights());
        LightSample ls;
        light->SampleLi(hitPoint, ls);
        // Create a new ray from the intersection point to the light source
        Ray shadowRay(hitPoint, ls.wi);
        // Check if the new ray intersects with any object in the scene
        Vector3f localWi = worldToNormal * ls.wi;
        if (ls.pdf != 0 && !primitives->intersectP(shadowRay, 0.001f)) {
            finalColor += ls.Li * (Vector3f::dot(ls.wi, normal)) / ls.pdf * material->f(localWo, localWi);
        }
        
        // Indirect lighting
        Vector3f randomDir = Vector3f(rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f).normalized();
        Vector3f localRandomDir = worldToNormal * randomDir;
        Ray sampleRay(hitPoint + normal * 1e-3, randomDir);
        finalColor += material->f(localWo, localRandomDir) * SampleL(scene, sampleRay, depth + 1);
    }
    else {
        // If it doesn't, account for the background color
        finalColor += scene.getBackgroundColor();
    }

    // Russian roulette
    finalColor /= survivalRate;

    return finalColor;
}