#include "integrator.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "light.hpp"

Vector3f Integrator::SampleL(const SceneParser& scene, Ray ray) {
    Hit hit;
    Group* objects = scene.getGroup();
    if (objects->intersect(ray, hit, 0)) {
        Vector3f color = Vector3f::ZERO;
        for (int li = 0; li < scene.getNumLights(); li++) {
            Light* light = scene.getLight(li);
            Vector3f L, lightColor;
            light->getIllumination(ray.pointAtParameter(hit.getT()), L, lightColor);
            color += hit.getMaterial()->Shade(ray, hit, L, lightColor);
        }
        return color;
    }
    else {
        return scene.getBackgroundColor();
    }
    
    return Vector3f(0, 0, 0);
}