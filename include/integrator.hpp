#ifndef INTEGRATOR_HPP
#define INTEGRATOR_HPP

#include "vecmath.h"
#include "ray.hpp"
#include "group.hpp"

class Integrator {
public:
    Integrator() {}
    ~Integrator() {}

    Vector3f SampleL(Group* scene, Ray ray);
};

#endif