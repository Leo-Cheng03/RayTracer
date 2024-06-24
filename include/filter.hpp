#ifndef FILTER_HPP
#define FILTER_HPP

#include <map>
#include <vecmath.h>
#include "sampler.hpp"


float Gaussian(float x, float mu = 0, float sigma = 1) {
    return 1.0f / std::sqrt(2 * M_PI) / sigma * std::exp(-0.5f * std::pow((x - mu) / sigma, 2));
}

float GaussianIntegral(float x0, float x1, float mu = 0, float sigma = 1) {
    float sigmaRootTwo = sigma * 1.41421356237095f;
    return 0.5f * (std::erf((mu - x0) / sigmaRootTwo) - std::erf((mu - x1) / sigmaRootTwo));
}


struct FilterSample
{
    Vector2f p;
    float weight;
};


// class FilterSampler {
// public:
//     FilterSampler(const Filter& filter, const Vector2f& radius, Sampler& _sampler);

//     FilterSample Sample() const;

// private:
//     Sampler& sampler;
//     const Filter& filter;
//     std::map<Vector2f, FilterSample> samples;
// };


class Filter {
public:
    Filter(const Vector2f& radius, float sigma, Sampler& _sampler)
        : radius(radius), sigma(sigma), expX(Gaussian(radius.x(), 0, sigma)), 
          expY(Gaussian(radius.y(), 0, sigma)) 
    {
        // 32 samples per pixel in each direction
        int xSize = int(32 * radius.x()); 
        int ySize = int(32 * radius.y());

        for (int y = 0; y < ySize; ++y)
            for (int x = 0; x < xSize; ++x) {
                Vector2f p = Vector2f(2 * radius.x() * (x + 0.5f) / xSize, 
                                      2 * radius.y() * (y + 0.5f) / ySize) -
                             Vector2f(radius.x(), radius.y());
                f[p] = Evaluate(p);
            }
    }

    Vector2f Radius() const { return radius; }

    float Evaluate(const Vector2f& p) const {
        return std::max(0.0f, Gaussian(p.x(), 0, sigma) - expX) * 
               std::max(0.0f, Gaussian(p.y(), 0, sigma) - expY);
    }
    float Integral() const {
        return (GaussianIntegral(-radius.x(), radius.x(), 0, sigma) - 2 * radius.x() * expX) * 
               (GaussianIntegral(-radius.y(), radius.y(), 0, sigma) - 2 * radius.y() * expY);
    }
    
    FilterSample Sample(const Vector2f& u) const {
        Vector2f p((u.x() - 0.5f) * 2 * radius.x(), (u.y() - 0.5f) * 2 * radius.y());
        return { p, Evaluate(p) / Integral() };
    }

private:
    Vector2f radius;
    float sigma, expX, expY;
    std::map<Vector2f, float> f;
    // FilterSampler sampler;
};

#endif // FILTER_HPP