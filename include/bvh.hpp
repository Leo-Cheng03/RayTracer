#ifndef BVH_HPP
#define BVH_HPP

#include "bound.hpp"
#include "object3d.hpp"
#include <span>
#include <vector>
#include <iostream>

struct BVHObject {
    BVHObject() {
        obj = nullptr;
        bound = Bound3f();
    }

    BVHObject(Object3D* _obj, const Bound3f& _bound) : 
        obj(_obj), bound(_bound) {}
    
    Object3D* obj;
    Bound3f bound;

    Vector3f Centroid() const { return (bound[0] + bound[1]) / 2; }
};

struct BVHNode {
    Bound3f bound;
    BVHNode* children[2];
    int splitAxis;
    std::vector<Object3D*> objects;

    ~BVHNode() {
        for (Object3D* obj : objects) {
            delete obj;
        }
        delete children[0];
        delete children[1];
    }

    void SetLeaf(std::span<BVHObject> bvhObjects, const Bound3f& b) {
        bound = b;
        children[0] = children[1] = nullptr;
        for (const BVHObject& obj : bvhObjects) {
            objects.push_back(obj.obj);
        }
    }

    void SetInterior(int axis, BVHNode* c0, BVHNode* c1) {
        children[0] = c0;
        children[1] = c1;
        bound = Bound3f::Union(c0->bound, c1->bound);
        splitAxis = axis;
    }
};

struct BVHSplitBucket {
    int count = 0;
    Bound3f bound;
};

class BVH {
public:
    BVH(std::vector<Object3D*> objs, int maxObjInNode = 255) : 
        maxObjInNode(maxObjInNode) {
        
        // Initialize bvhObjects array
        std::vector<BVHObject> bvhObjects(objs.size());
        for (size_t i = 0; i < objs.size(); i++) {
            bvhObjects[i] = BVHObject(objs[i], objs[i]->Bounds());
        }

        // Build BVH tree
        int totalNodes = 0;
        int orderedObjsOffset = 0;

        std::cout << "Building BVH tree..." << std::endl;
        std::cout << "Number of objects: " << objs.size() << std::endl;
        std::cout << "bvhObjects size: " << bvhObjects.size() << std::endl;

        root = Build(std::span<BVHObject>(bvhObjects), 
                     totalNodes, 
                     orderedObjsOffset
                    );

        std::cout << "BVH tree built." << std::endl;
        PrintTree();
    }

    ~BVH() {
        delete root;
    }

    Bound3f BVHBounds() const {
        return root->bound;
    }

    BVHNode* Build(std::span<BVHObject> bvhObjects, int& totalNodes, int& offset);
    bool Intersect(const Ray& ray, Hit& hit, float tmin) const;
    bool IntersectP(const Ray& ray,float tmin, float tmax) const;

    void PrintNode(BVHNode* node, int depth) const {
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        std::cout << "Node";
        if (!node->objects.empty()) std::cout << " (leaf: " << node->objects.size() << ")";
        std::cout << ": " << node->bound[0] << " | " << node->bound[1] << std::endl;
        if (node->children[0]) {
            PrintNode(node->children[0], depth + 1);
        }
        if (node->children[1]) {
            PrintNode(node->children[1], depth + 1);
        }
    }

    void PrintTree() const {
        std::cout << "--- BVH Tree ---" << std::endl;
        PrintNode(root, 0);
    }


private:
    int maxObjInNode;
    BVHNode* root;
};

#endif