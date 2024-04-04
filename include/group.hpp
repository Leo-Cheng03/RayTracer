#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        objects.reserve(num_objects);
    }

    ~Group() override {
        for (auto obj : objects) {
            delete obj;
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool hit = false;
        for (auto obj : objects) {
            if (obj && obj->intersect(r, h, tmin)) {
                hit = true;
            }
        }
        return hit;
    }

    void addObject(int index, Object3D *obj) {
        objects.push_back(obj);
    }

    int getGroupSize() {
        return objects.size();
    }

private:
    std::vector<Object3D*> objects;
};

#endif
	
