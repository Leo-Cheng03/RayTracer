#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "integrator.hpp"
#include "sampler.hpp"
#include "filter.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3) {
        cout << "Usage: ./bin/COURSEWORK <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];  // only bmp is allowed.

    // TODO: Main RayCasting Logic
    // First, parse the scene using SceneParser.
    SceneParser scene(inputFile.c_str());
    Image image(scene.getCamera()->getWidth(), scene.getCamera()->getHeight());
    Integrator integrator;
    StratifiedSampler sampler(2, 2, true);
    // RandomSampler sampler(4);
    Filter filter(Vector2f(0.5, 0.5), 0.5, sampler);
    // Then loop over each pixel in the image, shooting a ray

    std::cout << "start rendering...\n";

    for (int y = 0; y < scene.getCamera()->getHeight(); y++) {
    // for (int y = (image.Height() - 100 - 1); y < (image.Height() - 100); y++) {
        for (int x = 0; x < scene.getCamera()->getWidth(); x++) {
        // for (int x = 63; x < 64; x++) {
            // through that pixel and finding its intersection with
            // the scene.  Write the color at the intersection to that
            // pixel in your output image.
            Vector3f color = Vector3f::ZERO;
            float weightSum = 0.0f;
            for (int s = 0; s < sampler.SamplesPerPixel(); s++) {
                sampler.StartPixel(x, y, s);

                // std::cout << "Pixel: (" << x << ", " << y << ") Sample: " << s << "\n";
                FilterSample fs = filter.Sample(sampler.Get2D());
                // fs.weight = 1.0;
                weightSum += fs.weight;
                Ray ray = scene.getCamera()->generateRay(Vector2f(x, y) + fs.p + Vector2f(0.5, 0.5)); // TODO: use the Gaussian distribution.
                Group* baseGroup = scene.getGroup();
                Vector3f sampleColor = Vector3f::ZERO;

                // if (true) {
                if (x == 63 && y == image.Height() - 100 -1) {
                // if (x == 0 && y == 0) {
                    std::cout << std::endl;
                    sampleColor = integrator.SampleL(scene, ray, sampler, nullptr, true);
                    if (std::isnan(sampleColor.x()) || std::isnan(sampleColor.y()) || std::isnan(sampleColor.z())) {
                        weightSum -= fs.weight;
                        continue;
                    }
                    std::cout << "Final Color: " << sampleColor << std::endl << std::endl;
                    sampleColor *= fs.weight;
                    sampleColor.x() = std::clamp(sampleColor.x(), 0.0f, 1.0f);
                    sampleColor.y() = std::clamp(sampleColor.y(), 0.0f, 1.0f);
                    sampleColor.z() = std::clamp(sampleColor.z(), 0.0f, 1.0f);
                    color +=  sampleColor;

                    if (s == sampler.SamplesPerPixel() - 1) {
                        std::cout << "Final Final Color: " << color / weightSum << std::endl << std::endl;
                    }
                }
                else {
                    sampleColor = integrator.SampleL(scene, ray, sampler, nullptr, false) * fs.weight;
                    if (std::isnan(sampleColor.x()) || std::isnan(sampleColor.y()) || std::isnan(sampleColor.z())) {
                        weightSum -= fs.weight;
                        continue;
                    }
                    sampleColor *= fs.weight;
                    sampleColor.x() = std::clamp(sampleColor.x(), 0.0f, 1.0f);
                    sampleColor.y() = std::clamp(sampleColor.y(), 0.0f, 1.0f);
                    sampleColor.z() = std::clamp(sampleColor.z(), 0.0f, 1.0f);
                    color +=  sampleColor;
                }

                std::cout << std::fixed << std::setprecision(2);
                std::cout << "\rProgress: " << 
                            (float)((sampler.SamplesPerPixel()) * 
                            (y * scene.getCamera()->getWidth() + x - 1) + s) * 100 / 
                            (float)((scene.getCamera()->getWidth() * 
                                    scene.getCamera()->getHeight()) * 
                                    sampler.SamplesPerPixel()) << "% (" << x << ", " << y << ")" << std::flush;

                // std::cout << std::endl;
            }
            color /= weightSum;
            image.SetPixel(x, y, color);
        }
    }
    std::cout << "\rProgess: 100.00%\n" << std::endl;

    image.SaveBMP(outputFile.c_str());
    return 0;
}

