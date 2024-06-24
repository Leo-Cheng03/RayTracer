#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

bool Mesh::intersect(const Ray &r, Hit &h, float tmin) const {

    // Optional: Change this brute force method into a faster one.
    bool result = bvh->Intersect(r, h, tmin);
    return result;
}

bool Mesh::intersectP(const Ray &r, float tmin, float tmax) const {
    return bvh->IntersectP(r, tmin, tmax);
}

Bound3f Mesh::Bounds() const {
    Bound3f bound;
    for (const Vector3f& vec : v) {
        bound = Bound3f::Union(bound, vec);
    }
    return bound;
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material) {

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream f;
    f.open(filename);
    if (!f.is_open()) {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    int texID;
    while (true) {
        std::getline(f, line);
        if (f.eof()) {
            break;
        }
        if (line.size() < 3) {
            continue;
        }
        if (line.at(0) == '#') {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok) {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
        } else if (tok == fTok) {
            if (line.find(bslash) != std::string::npos) {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                TriangleIndex texCoords;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++) {
                    facess >> trig[ii] >> texCoords[ii];
                    trig[ii]--;
                    texCoords[ii]--;
                }
                t.push_back(trig);
                tex.push_back(texCoords);
            } else {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++) {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                t.push_back(trig);
            }
        } else if (tok == texTok) {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            vt.push_back(texcoord);
        }
    }
    computeNormal();

    f.close();

    std::vector<Object3D*> triangles;
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Triangle* triangle = new Triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], material);
        triangle->normal = n[triId];
        if (vt.size() > 0) {
            triangle->setTexCoord(vt[tex[triId][0]], vt[tex[triId][1]], vt[tex[triId][2]]);
        }
        else {
            triangle->setTexCoord(Vector2f(0, 0), Vector2f(0, 1), Vector2f(1, 1));
        }

        triangles.push_back(triangle);
    }
    bvh = new BVH(triangles);
}

void Mesh::computeNormal() {
    n.resize(t.size());
    for (int triId = 0; triId < (int) t.size(); ++triId) {
        TriangleIndex& triIndex = t[triId];
        Vector3f a = v[triIndex[1]] - v[triIndex[0]];
        Vector3f b = v[triIndex[2]] - v[triIndex[0]];
        b = Vector3f::cross(a, b);
        n[triId] = b / b.length();
    }
}
