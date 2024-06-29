#ifndef TEXTURESAMPLER_HPP
#define TEXTURESAMPLER_HPP

#include <string>
#include <iostream>
#include <Vector2f.h>
#include <Vector3f.h>
#include "image.hpp"
#include "hit.hpp"

struct MaterialSampleContext {
    // MaterialSampleContext(const Vector3f& position, const Vector3f& normal, const Vector3f& wo, const Vector2f& uv) :
    //     position(position), normal(normal), wo(wo), uv(uv) {}

    MaterialSampleContext(const Vector2f& uv) :
        uv(uv) {}

    MaterialSampleContext(const Hit& hit) :
        uv(hit.getUV()), normal(hit.getNormal()), tangent(hit.getTangent()),
        bitangent(hit.getBitangent()) {}
    
    Vector2f uv;
    Vector3f normal;
    Vector3f tangent;
    Vector3f bitangent;
};

class Vector3fSampler {
public:
    virtual Vector3f Sample(const MaterialSampleContext& context) = 0;
};

class FloatSampler {
public:
    virtual float Sample(const MaterialSampleContext& context) = 0;
};

class Vector3fConstant : public Vector3fSampler {
public:
    Vector3fConstant(const Vector3f& value) : value(value) {}

    Vector3f Sample(const MaterialSampleContext& context) override {
        return value;
    }

private:
    Vector3f value;
};

class Vector3fTexture : public Vector3fSampler {
public:
    Vector3fTexture(const std::string& filename) {
        if (filename.find(".bmp") != std::string::npos) {
            image = Image::LoadBMP(filename.c_str());
        }
        else if (filename.find(".tga") != std::string::npos) {
            image = Image::LoadTGA(filename.c_str());
        }
        else {
            std::cerr << "Unsupported texture format: " << filename << std::endl;
            exit(1);
        }
    }

    Vector3f Sample(const MaterialSampleContext& context) override {
        Vector2f uv = context.uv;
        int x = int(uv.x() * image->Width()) % image->Width();
        int y = int(uv.y() * image->Height()) % image->Height();
        x = x < 0 ? x + image->Width() : x;
        y = y < 0 ? y + image->Height() : y;
        // std::cout << "uv: " << uv << " map to texture " << x << ", " << y << std::endl;
        return image->GetPixel(x, y);
    }

private:
    Image* image;
};

class FloatConstant : public FloatSampler {
public:
    FloatConstant(float value) : value(value) {}

    float Sample(const MaterialSampleContext& context) override {
        return value;
    }

private:
    float value;
};

class FloatTexture : public FloatSampler {
public:
    FloatTexture(const std::string& filename, int _channel) : channel(_channel) {
        if (filename.find(".bmp") != std::string::npos) {
            image = Image::LoadBMP(filename.c_str());
        }
        else if (filename.find(".tga") != std::string::npos) {
            image = Image::LoadTGA(filename.c_str());
        }
        else {
            std::cerr << "Unsupported texture format: " << filename << std::endl;
            exit(1);
        }
    }

    float Sample(const MaterialSampleContext& context) override {
        Vector2f uv = context.uv;
        int x = int(uv.x() * image->Width()) % image->Width();
        int y = int(uv.y() * image->Height()) % image->Height();
        Vector3f pixel = image->GetPixel(x, y);
        return pixel[channel];
    }

private:
    Image* image;
    int channel;
};

#endif