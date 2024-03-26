#include "vecmath.hpp"

template <typename T>
template <typename U>
Vector2<T>::Vector2(const Point2<U>& p) : Tuple2<Vector2, T>(p.x, p.y) {}

template <typename T>
template <typename U>
Vector3<T>::Vector3(const Point3<U>& n) : Tuple3<Vector3, T>(n.x, n.y, n.z) {}

template <typename T>
template <typename U>
Vector3<T>::Vector3(const Normal3<U>& n) : Tuple3<Vector3, T>(static_cast<T>(n.x), static_cast<T>(n.y), static_cast<T>(n.z)) {}
