#ifndef POINT3D_H
#define POINT3D_H

#include <math.h>

struct Point3d
{
    float x, y, z;

    Point3d() noexcept
        : x(0)
        , y(0)
        , z(0)
    {
    }

    Point3d(float v_) noexcept
        : x(v_)
        , y(v_)
        , z(v_)
    {
    }

    Point3d(float x_, float y_) noexcept
        : x(x_)
        , y(y_)
        , z(0)
    {
    }

    Point3d(float x_, float y_, float z_) noexcept
        : x(x_)
        , y(y_)
        , z(z_)
    {
    }

    Point3d operator+(const Point3d &p) const
    {
        return Point3d(*this) += p;
    }

    Point3d operator-(const Point3d &p) const
    {
        return Point3d(*this) -= p;
    }

    Point3d operator*(float f) const
    {
        return Point3d(*this) *= f;
    }

    Point3d operator*(const Point3d &f) const noexcept
    {
        return Point3d(*this) *= f;
    }

    Point3d &operator*=(const Point3d &p) noexcept
    {
        x *= p.x;
        y *= p.y;
        z *= p.z;
        return *this;
    }

    Point3d &operator+=(const Point3d &p) noexcept
    {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }

    Point3d &operator-=(const Point3d &p) noexcept
    {
        x -= p.x;
        y -= p.y;
        z -= p.z;
        return *this;
    }

    bool operator==(const Point3d &f) const noexcept
    {
        return (x==f.x) && (y==f.y) && (z==f.z);
    }

    Point3d normalize() const
    {
        const float r = 1. / sqrt(x * x + y * y + z * z);
        return Point3d(x * r, y * r, z * r);
    }
    float &operator[](unsigned int index) noexcept {
        return (&x)[index];
    }

    const float &operator[](unsigned int index) const noexcept {
        return (&x)[index];
    }

    static float dot(const Point3d &a, const Point3d &b) noexcept
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Point3d cross(const Point3d &a, const Point3d &b) noexcept
    {
        return Point3d(a.y * b.z - a.z * b.y,
                       a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x);
    }

    float length() const
    {
        constexpr float x1 = 0;
        constexpr float y1 = 0;
        const float x2 = x;
        const float y2 = y;

        const float dy = x1 - x2;
        const float dz = y1 - y2;

        return sqrt((dz * dz) + (dy * dy));
    }
};


#endif // POINT3D_H
