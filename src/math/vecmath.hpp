#pragma once
#include <math.h>

using Float = float;

// Tuple2 -------------------------------------------------------
template <template <typename> class Child, typename T>
class Tuple2 {
public:
    T x{}, y{};

public:
    Tuple2(T x, T y) : x(x), y(y) {}

    T operator[](int i) const {
        if (i == 0) return x;
        return y;
    }

    T& operator[](int i) {
        if (i == 0) return x;
        return y;
    }

    template <typename U>
    auto operator+(const Child<U>& c) const -> Child<decltype(T() + U())> {
        return { x + c.x, y + c.y };
    }

    template <typename U>
    auto operator-(const Child<U>& c) const -> Child<decltype(T() - U())> {
        return { x - c.x, y - c.y };
    }

    template <typename U>
    Child<T> operator-() const {
        return { -x, -y };
    }

    template <typename U>
    auto operator*(U s) const -> Child<decltype(T() * U())> {
        return { x * s, y * s };
    }

    template <typename U>
    auto operator/(U s) const -> Child<decltype(T() / U())> {
        return { x / s, y / s };
    }

    template <typename U>
    auto operator*(const Child<U>& c) const -> Child<decltype(T() * U())> {
        return { x * c.x, y * c.y };
    }

    template <typename U>
    auto operator/(const Child<U>& c) const -> Child<decltype(T() / U())> {
        return { x / c.x, y / c.y };
    }

    template <typename U>
    auto operator+=(const Child<U>& c) -> Child<T>& {
        return { x += c.x, y += c.y };
    }

    template <typename U>
    auto operator-=(const Child<U>& c) -> Child<T>& {
        return { x -= c.x, y -= c.y };
    }

    template <typename U>
    auto operator*=(U s) -> Child<T>& {
        return { x *= s, y *= s };
    }

    template <typename U>
    auto operator/=(U s) -> Child<T>& {
        return { x /= s, y /= s };
    }

    template <typename U>
    auto operator*=(const Child<U>& c) -> Child<T>& {
        return { x *= c.x, y *= c.y };
    }

    template <typename U>
    auto operator/=(const Child<U>& c) -> Child<T>& {
        return { x /= c.x, y /= c.y };
    }
};

template <template <typename> class Child, typename T>
Child<T> Abs(const Child<T>& c) {
    return { abs(c.x), abs(c.y) };
}

template <template <typename> class Child, typename T>
Child<int> Ceil(const Child<T>& c) {
    return { ceil(c.x), ceil(c.y) };
}

template <template <typename> class Child, typename T>
Child<int> Floor(const Child<T>& c) {
    return { floor(c.x), floor(c.y) };
}

template <template <typename> class Child, typename T>
Child<float> Lerp(float t, const Child<T>& a, const Child<T>& b) {
    return (1 - t) * a + t * b;
}

template <template <typename> class Child, typename T>
Child<T> Min(const Child<T>& a, const Child<T>& b) {
    return { min(a.x, b.x), min(a.y, b.y) };
}

template <template <typename> class Child, typename T>
Child<T> Max(const Child<T>& a, const Child<T>& b) {
    return { max(a.x, b.x), max(a.y, b.y) };
}

template <template <typename> class Child, typename T>
T MinComponentValue(const Child<T>& c) {
    return min(c.x, c.y);
}

template <template <typename> class Child, typename T>
T MaxComponentValue(const Child<T>& c) {
    return max(c.x, c.y);
}

template <template <typename> class Child, typename T>
int MinComponentIndex(const Child<T>& c) {
    return c.x < c.y ? 0 : 1;
}

template <template <typename> class Child, typename T>
int MaxComponentIndex(const Child<T>& c) {
    return c.x > c.y ? 0 : 1;
}

template <template <typename> class Child, typename T>
Child<T> Permute(const Child<T>& c, int permute[]) {
    return { c[permute[0]], c[permute[1]] };
}

template <template <typename> class Child, typename T>
T HProd(const Child<T>& c) {
    return c.x * c.y;
}

// Tuple3 -------------------------------------------------------
template <template <typename> class Child, typename T>
class Tuple3 {
public:
    T x{}, y{}, z{};

public:
    Tuple3(T x, T y, T z) : x(x), y(y), z(z) {}

    T operator[](int i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    T& operator[](int i) {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    template <typename U>
    auto operator+(const Child<U>& c) const -> Child<decltype(T() + U())> {
        return { x + c.x, y + c.y, z + c.z };
    }

    template <typename U>
    auto operator-(const Child<U>& c) const -> Child<decltype(T() - U())> {
        return { x - c.x, y - c.y, z - c.z };
    }

    template <typename U>
    Child<T> operator-() const {
        return { -x, -y, -z };
    }

    template <typename U>
    auto operator*(U s) const -> Child<decltype(T() * U())> {
        return { x * s, y * s, z * s };
    }

    template <typename U>
    auto operator/(U s) const -> Child<decltype(T() / U())> {
        return { x / s, y / s, z / s };
    }

    template <typename U>
    auto operator*(const Child<U>& c) const -> Child<decltype(T() * U())> {
        return { x * c.x, y * c.y, z * c.z };
    }

    template <typename U>
    auto operator/(const Child<U>& c) const -> Child<decltype(T() / U())> {
        return { x / c.x, y / c.y, z / c.z };
    }

    template <typename U>
    auto operator+=(const Child<U>& c) -> Child<T>& {
        return { x += c.x, y += c.y, z += c.z };
    }

    template <typename U>
    auto operator-=(const Child<U>& c) -> Child<T>& {
        return { x -= c.x, y -= c.y, z -= c.z };
    }

    template <typename U>
    auto operator*=(U s) -> Child<T>& {
        return { x *= s, y *= s, z *= s };
    }

    template <typename U>
    auto operator/=(U s) -> Child<T>& {
        return { x /= s, y /= s, z /= s };
    }

    template <typename U>
    auto operator*=(const Child<U>& c) -> Child<T>& {
        return { x *= c.x, y *= c.y, z *= c.z };
    }

    template <typename U>
    auto operator/=(const Child<U>& c) -> Child<T>& {
        return { x /= c.x, y /= c.y, z /= c.z };
    }
};

template <template <typename> class Child, typename T>
Child<T> Abs(const Child<T>& c) {
    return { abs(c.x), abs(c.y), abs(c.z) };
}

template <template <typename> class Child, typename T>
Child<int> Ceil(const Child<T>& c) {
    return { ceil(c.x), ceil(c.y), ceil(c.z) };
}

template <template <typename> class Child, typename T>
Child<int> Floor(const Child<T>& c) {
    return { floor(c.x), floor(c.y), floor(c.z) };
}

template <template <typename> class Child, typename T>
Child<float> Lerp(float t, const Child<T>& a, const Child<T>& b) {
    return (1 - t) * a + t * b;
}

template <template <typename> class Child, typename T>
Child<T> Min(const Child<T>& a, const Child<T>& b) {
    return { min(a.x, b.x), min(a.y, b.y), min(a.z, b.z) };
}

template <template <typename> class Child, typename T>
Child<T> Max(const Child<T>& a, const Child<T>& b) {
    return { max(a.x, b.x), max(a.y, b.y), max(a.z, b.z) };
}

template <template <typename> class Child, typename T>
T MinComponentValue(const Child<T>& c) {
    return min(c.x, min(c.y, c.z));
}

template <template <typename> class Child, typename T>
T MaxComponentValue(const Child<T>& c) {
    return max(c.x, max(c.y, c.z));
}

template <template <typename> class Child, typename T>
int MinComponentIndex(const Child<T>& c) {
    if (c.x < c.y) {
        if (c.x < c.z) return 0;
        return 2;
    }
    if (c.y < c.z) return 1;
    return 2;
}

template <template <typename> class Child, typename T>
int MaxComponentIndex(const Child<T>& c) {
    if (c.x > c.y) {
        if (c.x > c.z) return 0;
        return 2;
    }
    if (c.y > c.z) return 1;
    return 2;
}

template <template <typename> class Child, typename T>
Child<T> Permute(const Child<T>& c, int permute[]) {
    return { c[permute[0]], c[permute[1]], c[permute[2]] };
}

template <template <typename> class Child, typename T>
T HProd(const Child<T>& c) {
    return c.x * c.y * c.z;
}

// Forward declarations
template <typename T> class Point2;
template <typename T> class Normal2;
template <typename T> class Point3;
template <typename T> class Normal3;

// Vector2 ------------------------------------------------------
template <typename T>
class Vector2 : public Tuple2<Vector2, T> {
public:
    Vector2(T x, T y) : Tuple2<Vector2, T>(x, y) {}

    template <typename U>
    Vector2(const Tuple2<Vector2, U>& v) : Tuple2<Vector2, T>(T(v.x), T(v.y)) {}

    template <typename U>
    explicit Vector2(const Point2<U>& p);

    template <typename U>
    explicit Vector2(const Normal2<U>& n);
};

template <typename T>
T LengthSquared(const Vector2<T>& v) {
    return v.x * v.x + v.y * v.y;
}

template <typename T>
Float Length(const Vector2<T>& v) {
    return std::sqrt(LengthSquared(v));
}

template <typename T>
Vector2<T> Normalize(const Vector2<T>& v) {
    return v / Length(v);
}

template <typename T>
T Dot(const Vector2<T>& a, const Vector2<T>& b) {
    return a.x * b.x + a.y * b.y;
}

using Vector2f = Vector2<Float>;
using Vector2i = Vector2<int>;

// Vector3 ------------------------------------------------------
template <typename T>
class Vector3 : public Tuple3<Vector3, T> {
public:
    Vector3(T x, T y, T z) : Tuple3<Vector3, T>(x, y, z) {}

    template <typename U>
    Vector3(const Tuple3<Vector3, U>& v) : Tuple3<Vector3, T>(T(v.x), T(v.y), T(v.z)) {}

    template <typename U>
    explicit Vector3(const Point3<U>& p);

    template <typename U>
    explicit Vector3(const Normal3<T>& n);
};

template <typename T>
T LengthSquared(const Vector3<T>& v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

template <typename T>
Float Length(const Vector3<T>& v) {
    return std::sqrt(LengthSquared(v));
}

template <typename T>
Vector3<T> Normalize(const Vector3<T>& v) {
    return v / Length(v);
}

template <typename T>
T Dot(const Vector3<T>& a, const Vector3<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
Vector3<T> Cross(const Vector3<T>& a, const Vector3<T>& b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

using Vector3f = Vector3<Float>;
using Vector3i = Vector3<int>;

// Point2 -------------------------------------------------------
template <typename T>
class Point2: public Tuple2<Point2, T> {
public:
    Point2(T x, T y) : Tuple2<Poiint2, T>(x, y) {}

    template <typename U>
    Point2(const Tuple2<Point2, U>& p) : Tuple2<Point2, T>(T(p.x), T(p.y)) {}

    template <typename U>
    explicit Point2(const Vector2<U>& v) : Tuple2<Point2, T>(T(v.x), T(v.y)) {}

    template <typename U>
    auto operator+(const Vector2<U>& v) const -> Point2<decltype(T() + U())> {
        return { x + v.x, y + v.y };
    }

    template <typename U>
    auto operator-(const Vector2<U>& v) const -> Point2<decltype(T() - U())> {
        return { x - v.x, y - v.y };
    }

    template <typename U>
    auto operator+=(const Vector2<U>& v) -> Point2<T>& {
        x += v.x;
        y += v.y;
        return *this;
    }

    template <typename U>
    auto operator-=(const Vector2<U>& v) -> Point2<T>& {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    template <typename U>
    auto operator-(const Point2<U>& p) const -> Vector2<decltype(T() - U())> {
        return { x - p.x, y - p.y };
    }
};

template <typename T>
auto Distance(const Point2<T>& p1, const Point2<T>& p2) -> T {
    return Length(p1 - p2);
}

template <typename T>
auto DistanceSquared(const Point2<T>& p1, const Point2<T>& p2) -> T {
    return LengthSquared(p1 - p2);
}

using Point2f = Point2<Float>;
using Point2i = Point2<int>;

// Point3 -------------------------------------------------------
template <typename T>
class Point3: public Tuple3<Point3, T> {
public:
    Point3(T x, T y, T z) : Tuple3<Point3, T>(x, y, z) {}

    template <typename U>
    Point3(const Tuple3<Point3, U>& p) : Tuple3<Point3, T>(T(p.x), T(p.y), T(p.z)) {}
    
    template <typename U>
    explicit Point3(const Vector3<U>& v) : Tuple3<Point3, T>(T(v.x), T(v.y), T(v.z)) {}

    template <typename U>
    auto operator+(const Vector3<U>& v) const -> Point3<decltype(T() + U())> {
        return { x + v.x, y + v.y, z + v.z };
    }

    template <typename U>
    auto operator-(const Vector3<U>& v) const -> Point3<decltype(T() - U())> {
        return { x - v.x, y - v.y, z - v.z };
    }

    template <typename U>
    auto operator+=(const Vector3<U>& v) -> Point3<T>& {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    template <typename U>
    auto operator-=(const Vector3<U>& v) -> Point3<T>& {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    template <typename U>
    auto operator-(const Point3<U>& p) const -> Vector3<decltype(T() - U())> {
        return { x - p.x, y - p.y, z - p.z };
    }
};

template <typename T>
auto Distance(const Point3<T>& p1, const Point3<T>& p2) -> T {
    return Length(p1 - p2);
}

template <typename T>
auto DistanceSquared(const Point3<T>& p1, const Point3<T>& p2) -> T {
    return LengthSquared(p1 - p2);
}

using RGB = Point3<Float>;
using Point3f = Point3<Float>;
using Point3i = Point3<int>;