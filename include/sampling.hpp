#ifndef SAMPLING_HPP
#define SAMPLING_HPP

#include "vecmath.h"
#include <vector>
#include <span>

template <typename Predicate>
size_t FindInterval(size_t sz, const Predicate &pred) {
    using ssize_t = std::make_signed_t<size_t>;
    ssize_t size = (ssize_t)sz - 2, first = 1;
    while (size > 0) {
        size_t half = (size_t)size >> 1, middle = first + half;
        bool predResult = pred(middle);
        first = predResult ? middle + 1 : first;
        size = predResult ? size - (half + 1) : half;
    }
    return (size_t)Clamp((ssize_t)first - 1, 0, sz - 2);
}

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

class PieceWiseConstant1D {
public:
    PieceWiseConstant1D(const std::vector<float>& values, float min, float max) : 
        min(min), max(max), func(values), cdf(values.size() + 1)
    {
        cdf[0] = values[0];
        for (int i = 1; i < func.size() + 1; i++) {
            cdf[i] = cdf[i - 1] + func[i - 1] * (max - min) / func.size();
        }

        funcInt = cdf.back();
        if (funcInt == 0)
            for (int i = 0; i < cdf.size(); i++)
                cdf[i] = i / (float)func.size(); // Uniform distribution
        else
            for (int i = 0; i < cdf.size(); i++)
                cdf[i] /= funcInt; // Normalize
    }

    float Integral() const { return funcInt; }
    size_t size() const { return func.size(); }

    float Sample(float u, float* pdf = nullptr, int* offset = nullptr) const {
        int o = FindInterval(func.size(), [&](size_t i) { return cdf[i] <= u; });

        if (offset) *offset = o;

        float du = u - cdf[o];
        if (cdf[o + 1] - cdf[o] > 0)
            du /= cdf[o + 1] - cdf[o];

        if (pdf)
            *pdf = (funcInt > 0) ? func[o] / funcInt : 0;
        
        return min + (o + du) / size() * (max - min);
    }

private:
    std::vector<float> func, cdf;
    float min, max;
    float funcInt = 0;
};

class PieceWiseConstant2D {
public:
    // PieceWiseConstant2D(const std::span<float>& values, int nu, int nv, const Bound2f& domain) : 
    //     domain(domain), pConditionalV(values.size())
    // {
    //     for (int i = 0; i < values.size(); i++)
    //         pConditionalV[i] = PieceWiseConstant1D(values[i], domain.pMin.y(), domain.pMax.y());

    //     std::vector<float> marginalFunc;
    //     for (int i = 0; i < values.size(); i++)
    //         marginalFunc.push_back(pConditionalV[i].Integral());
        
    //     pMarginal = PieceWiseConstant1D(marginalFunc, domain.pMin.x(), domain.pMax.x());
    // }

private:
    Bound2f domain;
    std::vector<PieceWiseConstant1D> pConditionalV;
    PieceWiseConstant1D pMarginal;
};

#endif