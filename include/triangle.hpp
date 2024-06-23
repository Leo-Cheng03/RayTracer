#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) const override {
		Vector3f edge1, edge2;
		Vector3f pvec, tvec, qvec;
		float det, inv_det, u, v, t;

		edge1 = vertices[1] - vertices[0];
		edge2 = vertices[2] - vertices[0];
		pvec = Vector3f::cross(ray.getDirection(), edge2);
		det = Vector3f::dot(edge1, pvec);

		if (det < 1e-6 && det > -1e-6) return false;
		
		inv_det = 1.0 / det;
		tvec = ray.getOrigin() - vertices[0];
		u = Vector3f::dot(tvec, pvec) * inv_det;

		if (u < 0 || u > 1) return false;

		qvec = Vector3f::cross(tvec, edge1);
		v = Vector3f::dot(ray.getDirection(), qvec) * inv_det;

		if (v < 0 || u + v > 1) return false;

		t = Vector3f::dot(edge2, qvec) * inv_det;

		if (t < tmin || t > hit.getT()) return false;

		hit.set(t, material, normal);
		return true;
	}

	bool intersectP(const Ray& ray, float tmin, float tmax) const override {
		// if (material->getBSDF()->IsTransparent()) return false;
		
		Vector3f edge1, edge2;
		Vector3f pvec, tvec, qvec;
		float det, inv_det, u, v, t;

		edge1 = vertices[1] - vertices[0];
		edge2 = vertices[2] - vertices[0];
		pvec = Vector3f::cross(ray.getDirection(), edge2);
		det = Vector3f::dot(edge1, pvec);

		if (det < 1e-6 && det > -1e-6) return false;
		
		inv_det = 1.0 / det;
		tvec = ray.getOrigin() - vertices[0];
		u = Vector3f::dot(tvec, pvec) * inv_det;
		if (u < 0 || u > 1) return false;

		qvec = Vector3f::cross(tvec, edge1);
		v = Vector3f::dot(ray.getDirection(), qvec) * inv_det;
		if (v < 0 || u + v > 1) return false;

		t = Vector3f::dot(edge2, qvec) * inv_det;
		if (t < tmin || t >= tmax) return false;

		return true;
	}

	Bound3f Bounds() const override {
		Bound3f bound = Bound3f(vertices[0]);
		for (int i = 1; i < 3; i++) {
			bound = Bound3f::Union(bound, vertices[i]);
		}
		return bound;
	}

	Vector3f normal;
	Vector3f vertices[3];
protected:

};

#endif //TRIANGLE_H
