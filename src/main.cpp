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
    StratifiedSampler sampler(4, 4, true);
    // RandomSampler sampler(4);
    // Then loop over each pixel in the image, shooting a ray

    for (int y = 0; y < scene.getCamera()->getHeight(); y++) {
        for (int x = 0; x < scene.getCamera()->getWidth(); x++) {
            // through that pixel and finding its intersection with
            // the scene.  Write the color at the intersection to that
            // pixel in your output image.
            Vector3f color = Vector3f::ZERO;
            for (int s = 0; s < sampler.SamplesPerPixel(); s++) {
                sampler.StartPixel(x, y, s);

                Ray ray = scene.getCamera()->generateRay(Vector2f(x, y) + sampler.Get2D());
                Group* baseGroup = scene.getGroup();

                if (x == 107 && y == image.Height() - 198) {
                    std::cout << std::endl;
                    color += integrator.SampleL(scene, ray, sampler, true);
                    std::cout << "Final Color: " << color << std::endl << std::endl;

                    if (s == sampler.SamplesPerPixel() - 1) {
                        std::cout << "Final Final Color: " << color / sampler.SamplesPerPixel() << std::endl << std::endl;
                    }

                }
                else {
                    color += integrator.SampleL(scene, ray, sampler, false);
                }

                std::cout << std::fixed << std::setprecision(2);
                std::cout << "\rProgess: " << (float)((sampler.SamplesPerPixel()) * (y * scene.getCamera()->getWidth() + x - 1) + s) * 100 / (float)((scene.getCamera()->getWidth() * scene.getCamera()->getHeight()) * sampler.SamplesPerPixel()) << "%" << std::flush;
            }
            color /= sampler.SamplesPerPixel();
            image.SetPixel(x, y, color);
        }
    }
    std::cout << "\rProgess: 100.00%\n" << std::endl;

    image.SaveBMP(outputFile.c_str());
    return 0;
}

