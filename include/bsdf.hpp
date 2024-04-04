#ifndef BSDF_HPP
#define BSDF_HPP

#include <vecmath.h>

struct BSDFSample {
    Vector3f f;
    Vector3f wi;
    float pdf;
    float eta;
};

class BSDF {
public:
    BSDF() {}
    ~BSDF() = default;

    virtual Vector3f f(Vector3f wo, Vector3f wi) = 0;
    virtual BSDFSample Sample_f(Vector3f wo) = 0;

protected:
};

#endif