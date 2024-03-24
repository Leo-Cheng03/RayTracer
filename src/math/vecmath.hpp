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
