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

		Vector2f uv = texCoords[0] + 
						u * (texCoords[1] - texCoords[0]) + 
						v * (texCoords[2] - texCoords[0]);

		hit.set(t, material, normal, uv, tangent, bitangent);
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

	void setTexCoord(const Vector2f& a, const Vector2f& b, const Vector2f& c) {
		texCoords[0] = a;
		texCoords[1] = b;
		texCoords[2] = c;

		Vector3f edge1 = vertices[1] - vertices[0];
		Vector3f edge2 = vertices[2] - vertices[0];

		Vector2f deltaUV1 = texCoords[1] - texCoords[0];
		Vector2f deltaUV2 = texCoords[2] - texCoords[0];

		float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());

		tangent = (f * (deltaUV2.y() * edge1 - deltaUV1.y() * edge2)).normalized();
		bitangent = (f * (-deltaUV2.x() * edge1 + deltaUV1.x() * edge2)).normalized();
	}

	Vector3f normal;
	Vector3f tangent;
	Vector3f bitangent;
	Vector3f vertices[3];
	Vector2f texCoords[3];
protected:

};

#endif //TRIANGLE_H
