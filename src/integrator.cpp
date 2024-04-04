#include "integrator.hpp"
#include "group.hpp"
#include "hit.hpp"
#include "light.hpp"

float russianRoulette(float p) {
    return (float)rand() / RAND_MAX < p;
}

Vector3f Integrator::SampleL(const SceneParser& scene, Ray ray) {
    Group* primitives = scene.getGroup();
    Hit hit;
    Vector3f finalColor = Vector3f::ZERO;
    int depth = 0;

    while (depth < 10) {
        Vector3f color = Vector3f::ZERO;

        // Check if the ray intersects with any object in the scene
        // If it does, get the surface normal and material properties
            // Direct lighting
            // Sample a light based on power heuristic
            // Create a new ray from the intersection point to the light source
            // Check if the new ray intersects with any object in the scene
            // If it doesn't, calculate the direct lighting
            
            // Indirect lighting
            // If the material is a light source
                // Return black
            // Otherwise, calculate the direct lighting
            // Create new rays based on the material properties
        // If it doesn't, account for the background color

        // Russian roulette
        if (depth > 3) {
            if (!russianRoulette(0.8f)) {
                break;
            }
            else {
                color /= 0.8f;
            }
        }
    }

    return Vector3f(0, 0, 0);
}