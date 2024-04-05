#ifndef INTEGRATOR_HPP
#define INTEGRATOR_HPP

#include "vecmath.h"
#include "ray.hpp"
#include "scene_parser.hpp"

class Integrator {
public:
    Integrator() {}
    ~Integrator() {}

    Vector3f SampleL(const SceneParser& scene, const Ray& ray, int depth = 0);
};

#endif