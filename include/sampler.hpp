#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include "vecmath.h"
#include <random>

inline int HashPixel(int x, int y, int seed) {
    return ((x * 73856093u) ^ (y * 19349663u) ^ (seed * 83492791u)) % 1000000007;
}

inline int HashPixelWithDimension(int x, int y, int dimension, int seed) {
    return ((x * 73856093u) ^ (y * 19349663u) ^ (dimension * 83492791u) ^ (seed * 83492791u)) % 1000000007;
}

inline int GetStratum(uint32_t index, uint32_t samples, uint32_t seed) {
    uint32_t w = samples - 1;
    w |= w >> 1;
    w |= w >> 2;
    w |= w >> 4;
    w |= w >> 8;
    w |= w >> 16;
    do {
        index ^= seed;
        index *= 0xe170893d;
        index ^= seed >> 16;
        index ^= (index & w) >> 4;
        index ^= seed >> 8;
        index *= 0x0929eb3f;
        index ^= seed >> 23;
        index ^= (index & w) >> 1;
        index *= 1 | seed >> 27;
        index *= 0x6935fa69;
        index ^= (index & w) >> 11;
        index *= 0x74dcb303;
        index ^= (index & w) >> 2;
        index *= 0x9e501cc3;
        index ^= (index & w) >> 2;
        index *= 0xc860a3df;
        index &= w;
        index ^= index >> 5;
    } while (index >= samples);
    return (index + seed) % samples;
}

class Sampler {
public:
    virtual int SamplesPerPixel() const = 0;
    virtual void StartPixel(int px, int py, int sampleIndex, int dimension = 0) = 0;
    virtual float Get1D() = 0;
    virtual Vector2f Get2D() = 0;
};

class RandomSampler : public Sampler {
public:
    RandomSampler(int samplesPerPixel, int seed = 0) : samplesPerPixel(samplesPerPixel), seed(seed) {
        distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }

    int SamplesPerPixel() const override { return samplesPerPixel; }

    void StartPixel(int px, int py, int sampleIndex, int dimension = 0) override {
        generator = std::default_random_engine(HashPixel(px, py, seed));
        generator.discard(sampleIndex * 65536ull + dimension);
    }

    float Get1D() override {
        return distribution(generator);
    }

    Vector2f Get2D() override {
        return { distribution(generator), distribution(generator) };
    }

private:
    int samplesPerPixel;
    int seed;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
};

class StratifiedSampler : public Sampler {
public:
    StratifiedSampler(int xSamples, int ySamples, bool jitter, int seed = 0) :
        xSamples(xSamples), ySamples(ySamples), jitter(jitter), seed(seed) {
        distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }

    int SamplesPerPixel() const override { return xSamples * ySamples; }

    void StartPixel(int px, int py, int sampleIndex, int dimension = 0) override {
        this->sampleIndex = sampleIndex;
        this->dimension = dimension;
        pixel[0] = px;
        pixel[1] = py;
        
        generator = std::default_random_engine(HashPixel(px, py, seed));
        generator.discard(sampleIndex * 65536ull + dimension);
    }

    float Get1D() override { 
        int stratum = GetStratum(
            sampleIndex, SamplesPerPixel(), 
            HashPixelWithDimension(pixel[0], pixel[1], dimension, seed)
        );
        ++dimension;
        float delta = jitter ? distribution(generator) : 0.5f;
        return (stratum + delta) / SamplesPerPixel();
    }

    Vector2f Get2D() override {
        int stratum = GetStratum(
            sampleIndex, SamplesPerPixel(), 
            HashPixelWithDimension(pixel[0], pixel[1], dimension, seed)
        );
        dimension += 2;
        int x = stratum % xSamples;
        int y = stratum / xSamples;
        float dx = jitter ? distribution(generator) : 0.5f;
        float dy = jitter ? distribution(generator) : 0.5f;
        return { (x + dx) / xSamples, (y + dy) / ySamples };
    }

private:
    int xSamples, ySamples;
    bool jitter;
    int seed;
    int pixel[2];
    int sampleIndex = 0;
    int dimension = 0;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution;
};

#endif