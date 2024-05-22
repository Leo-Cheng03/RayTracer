#ifndef SAMPLING_HPP
#define SAMPLING_HPP

#include "vecmath.h"

inline float GetUniform1D() {
    return (float)rand() / RAND_MAX;
}

inline Vector2f GetUniform2D() {
    return { GetUniform1D(), GetUniform1D() };
}

inline Vector3f SampleCosineHemisphere(Vector2f u) {
    float sin_theta = sqrt(u.x());
    float phi = 2 * M_PI * u.y();
    
    return Vector3f(sin_theta * sin(phi), sqrt(1 - u.x()), sin_theta * cos(phi));
}

inline float CosineHemispherePDF(float cos_theta) {
    return cos_theta / M_PI;
}

#endif