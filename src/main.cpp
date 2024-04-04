#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"
#include "integrator.hpp"

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
    // Then loop over each pixel in the image, shooting a ray

    for (int x = 0; x < scene.getCamera()->getWidth(); x++) {
        for (int y = 0; y < scene.getCamera()->getHeight(); y++) {
            // through that pixel and finding its intersection with
            // the scene.  Write the color at the intersection to that
            // pixel in your output image.
            Ray ray = scene.getCamera()->generateRay(Vector2f(x, y));
            Group* baseGroup = scene.getGroup();
            // if (baseGroup->intersect(ray, hit, 0)) {
            //     Vector3f color = Vector3f::ZERO;
            //     for (int li = 0; li < scene.getNumLights(); li++) {
            //         Light* light = scene.getLight(li);
            //         Vector3f L, lightColor;
            //         light->getIllumination(ray.pointAtParameter(hit.getT()), L, lightColor);
            //         color += hit.getMaterial()->Shade(ray, hit, L, lightColor);
            //     }
            //     image.SetPixel(x, y, color);
            // }
            // else {
            //     image.SetPixel(x, y, scene.getBackgroundColor());
            // }
            Vector3f color = integrator.SampleL(scene, ray);
            image.SetPixel(x, y, color);
        }
    }

    image.SaveBMP(outputFile.c_str());
    return 0;
}

