#include "bvh.hpp"
#include <algorithm>
#include <iostream>

BVHNode * BVH::Build(std::span<BVHObject> bvhObjects, int & totalNodes, int & offset)
{
    if (bvhObjects.size() == 0) return nullptr;

    BVHNode* node = new BVHNode();
    totalNodes++;

    // std::cout << "Building BVH node " << totalNodes << std::endl;
    // std::cout << "Number of objects: " << bvhObjects.size() << std::endl;
    // std::cout << "Offset: " << offset << std::endl;

    // Surface Area Heuristic
    // 1. Compute the bounding box of all objects' centroids
    Bound3f centroidBound;
    for (const BVHObject& obj : bvhObjects) {
        centroidBound = Bound3f::Union(centroidBound, obj.Centroid());
    }

    // std::cout << "step 1 end" << std::endl;
    // std::cout << "Bound: " << centroidBound[0] << " | " << centroidBound[1] << "\n";

    // 2. Find the axis with the largest extent
    int splitAxis = centroidBound.MaxDimension();

    if (centroidBound.pMax[splitAxis] == centroidBound.pMin[splitAxis]) {
        // If the remaining objects cannot be split by their centroids, 
        // create leaf node
        Bound3f bound;
        for (const BVHObject& obj : bvhObjects) {
            bound = Bound3f::Union(bound, obj.bound);
        }
        node->SetLeaf(bvhObjects, bound);
        return node;
    }

    // std::cout << "step 2 end" << std::endl;

    // 3. Set the buckets for calculating the cost later
    constexpr int nBuckets = 12;
    BVHSplitBucket buckets[nBuckets];

    for (const BVHObject& obj : bvhObjects) {
        int b = nBuckets * centroidBound.Offset(obj.Centroid())[splitAxis];
        if (b == nBuckets) {
            b = nBuckets - 1;
        }
        buckets[b].count++;
        buckets[b].bound = Bound3f::Union(buckets[b].bound, obj.bound);
    }

    // std::cout << "step 3 end" << std::endl;

    // 4. Calculate the cost of each split
    float cost[nBuckets - 1];
    int nSplit = nBuckets - 1;

    Bound3f boundBefore, boundAfter;
    int countBefore = 0, countAfter = 0;
    for (int i = 0; i < nSplit; i++) {
        boundBefore = Bound3f::Union(boundBefore, buckets[i].bound);
        countBefore += buckets[i].count;

        cost[i] = countBefore * boundBefore.SurfaceArea();
    }

    for (int i = nSplit; i > 0; i--) {
        boundAfter = Bound3f::Union(boundAfter, buckets[i].bound);
        countAfter += buckets[i].count;

        cost[i - 1] += countAfter * boundAfter.SurfaceArea();
    }

    // std::cout << "step 4 end" << std::endl;

    // 5. Find the split with the minimum cost
    int minCostSplit = -1;
    float minCost = INFINITY;
    for (int i = 0; i < nSplit; i++) {
        if (cost[i] < minCost) {
            minCost = cost[i];
            minCostSplit = i;
        }
    }

    float leafCost = bvhObjects.size();
    minCost = 0.5f + minCost / centroidBound.SurfaceArea();

    if (bvhObjects.size() <= maxObjInNode && minCost >= leafCost) {
        Bound3f bound;
        for (const BVHObject& obj : bvhObjects) {
            bound = Bound3f::Union(bound, obj.bound);
        }
        node->SetLeaf(bvhObjects, bound);
    }

    // std::cout << "step 5 end" << std::endl;

    // 6. Partition objects into two parts based on the split
    auto midIter = std::partition(bvhObjects.begin(), bvhObjects.end(),
        [&](const BVHObject& obj) {
            int b = nBuckets * centroidBound.Offset(obj.Centroid())[splitAxis];
            if (b == nBuckets) b--;
            return b <= minCostSplit;
        }
    );
    int mid = midIter - bvhObjects.begin();

    // std::cout << "step 6 end" << std::endl;

    // 7. Recursively build the left and right children
    BVHNode* left = Build(bvhObjects.subspan(0, mid), totalNodes, offset);
    BVHNode* right = Build(bvhObjects.subspan(mid), totalNodes, offset);
    node->SetInterior(splitAxis, left, right);

    // std::cout << "step 7 end" << std::endl;

    return node;
}

bool BVH::Intersect(const Ray &ray, Hit &hit, float tmin) const
{
    // std::cout << "Intersecting ray with BVH: " << this << std::endl;

    Vector3f invDir = Vector3f(1.f / ray.getDirection().x(), 
                               1.f / ray.getDirection().y(), 
                               1.f / ray.getDirection().z());
    int dirIsNeg[3] = {invDir.x() < 0, invDir.y() < 0, invDir.z() < 0};

    BVHNode* node = root;
    std::vector<BVHNode*> nodeStack;

    // std::cout << "root node: " << node->bound[0] << " | " << node->bound[1] << std::endl;
    // std::cout << "ray origin: " << ray.getOrigin() << std::endl;
    // std::cout << "ray direction" << ray.getDirection() << std::endl;

    bool isHit = false;

    while(true) {
        if (!node) break;
        if (node->bound.IntersectP(ray, tmin, INFINITY, invDir, dirIsNeg)) {
            if (node->objects.size() > 0) {
                // Leaf node
                // std::cout << "Leaf node (" << node->objects.size() << ")" << "(" << node->bound[0] << " | " << node->bound[1] << ")" << std::endl;                
                // for (int i = 0; i < node->objects.size(); i++) {
                //     if (objects[node->firstObjOffset + i]->intersect(ray, hit, 0)) {
                //         std::cout << "Hit" << std::endl;
                //         return true;
                //     }
                // }
                for (const Object3D* obj : node->objects) {
                    if (obj->intersect(ray, hit, tmin)) {
                        // std::cout << "Hit" << std::endl;
                        isHit = true;
                    }
                }
                // std::cout << "No hit" << std::endl;
            }
            else {
                // Interior node
                // std::cout << "Interior node (" << node->bound[0] << " , " << node->bound[1] << ")" << std::endl;
                if (dirIsNeg[node->splitAxis]) {
                    // When dirIsNeg is true, children[1] is closer
                    // std::cout << "visit right and push left" << std::endl;
                    nodeStack.push_back(node->children[0]);
                    node = node->children[1];
                } 
                else {
                    // std::cout << "visit left and push right" << std::endl;
                    nodeStack.push_back(node->children[1]);
                    node = node->children[0];
                }
                continue;
            }
        }
        else {
            // std::cout << "  No intersection" << std::endl;
        }
        if (nodeStack.empty()) break;
        node = nodeStack.back();
        nodeStack.pop_back();
    }
    return isHit;
}

bool BVH::IntersectP(const Ray &ray, float tmin, float tmax) const
{
    Vector3f invDir = Vector3f(1.f / ray.getDirection().x(), 
                               1.f / ray.getDirection().y(), 
                               1.f / ray.getDirection().z());
    int dirIsNeg[3] = {invDir.x() < 0, invDir.y() < 0, invDir.z() < 0};

    BVHNode* node = root;
    std::vector<BVHNode*> nodeStack;

    while(true) {
        if (!node) break;
        if (node->bound.IntersectP(ray, tmin, tmax, invDir, dirIsNeg)) {
            if (node->objects.size() > 0) {
                // Leaf node
                // for (int i = 0; i < node->numObjs; i++) {
                //     if (objects[node->firstObjOffset + i]->intersectP(ray, 0, INFINITY)) {
                //         return true;
                //     }
                // }
                for (const Object3D* obj : node->objects) {
                    if (obj->intersectP(ray, tmin, tmax)) {
                        return true;
                    }
                }
            } 
            else {
                // Interior node
                if (dirIsNeg[node->splitAxis]) {
                    // When dirIsNeg is true, children[1] is closer
                    nodeStack.push_back(node->children[0]);
                    node = node->children[1];
                } 
                else {
                    nodeStack.push_back(node->children[1]);
                    node = node->children[0];
                }
                continue;
            }
        } 
        if (nodeStack.empty()) break;
        node = nodeStack.back();
        nodeStack.pop_back();
    }

    return false;
}
