#pragma once
#include <cmath>
#include <iosfwd>
#include "ImGui/imgui.h"

struct Vec3
{
    float x, y, z;

    // Constructors
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Addition
    Vec3 operator+(const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
    Vec3& operator+=(const Vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }

    // Subtraction
    Vec3 operator-(const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
    Vec3& operator-=(const Vec3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }

    // Multiplication (component-wise)
    Vec3 operator*(const Vec3& rhs) const { return Vec3(x * rhs.x, y * rhs.y, z * rhs.z); }
    Vec3& operator*=(const Vec3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }

    // Multiplication by scalar
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }

    // Division by scalar
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    Vec3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    // Unary minus
    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    // Comparison
    bool operator==(const Vec3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
    bool operator!=(const Vec3& rhs) const { return !(*this == rhs); }

    // Dot product
    float Dot(const Vec3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

    // Cross product
    Vec3 Cross(const Vec3& rhs) const {
        return Vec3(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }

    // Length (magnitude)
    float Length() const { return std::sqrt(x * x + y * y + z * z); }

    // Normalize
    Vec3 Normalized() const {
        float len = Length();
        if (len == 0) return Vec3(0, 0, 0);
        return *this / len;
    }

    float Distance(const Vec3& other) const
    {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    //// Stream output
    //friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    //    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    //    return os;
    //}
};

// Aliases to override other naming conventions
using vec3 = Vec3;
using Vector3 = Vec3;
using VECTOR3 = Vec3;

struct Vec4
{
    float x, y, z, w;

    // Constructors
    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // Addition
    Vec4 operator+(const Vec4& rhs) const { return Vec4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
    Vec4& operator+=(const Vec4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }

    // Subtraction
    Vec4 operator-(const Vec4& rhs) const { return Vec4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
    Vec4& operator-=(const Vec4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }

    // Multiplication (component-wise)
    Vec4 operator*(const Vec4& rhs) const { return Vec4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
    Vec4& operator*=(const Vec4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }

    // Multiplication by scalar
    Vec4 operator*(float scalar) const { return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); }
    Vec4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }

    // Division by scalar
    Vec4 operator/(float scalar) const { return Vec4(x / scalar, y / scalar, z / scalar, w / scalar); }
    Vec4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

    // Unary minus
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }

    // Comparison
    bool operator==(const Vec4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
    bool operator!=(const Vec4& rhs) const { return !(*this == rhs); }

    // Dot product
    float Dot(const Vec4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w; }

    // Length (magnitude)
    float Length() const { return std::sqrt(x * x + y * y + z * z + w * w); }

    // Normalize
    Vec4 Normalized() const {
        float len = Length();
        if (len == 0) return Vec4(0, 0, 0, 0);
        return *this / len;
    }

    // Stream output
    //friend std::ostream& operator<<(std::ostream& os, const Vec4& v) {
    //    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    //    return os;
    //}
};

// Aliases to override other naming conventions
using vec4 = Vec4;
using Vector4 = Vec4;
using VECTOR4 = Vec4;


struct Vec2
{
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}

    ImVec2 ToImVec2()
    {
        return ImVec2(x, y);
    }
};

using vec2 = Vec2;
using Vector2 = Vec2;
using VECTOR2 = Vec2;

inline BOOL WorldToScreen(const Vec3& pos, Vec3& screen,
    const float m[16], int width, int height)
{
    Vec4 clip;
    clip.x = m[0] * pos.x + m[4] * pos.y + m[8] * pos.z + m[12];
    clip.y = m[1] * pos.x + m[5] * pos.y + m[9] * pos.z + m[13];
    clip.z = m[2] * pos.x + m[6] * pos.y + m[10] * pos.z + m[14];
    clip.w = m[3] * pos.x + m[7] * pos.y + m[11] * pos.z + m[15];

    if (clip.w <= 0.0f) return false; // behind camera

    float invW = 1.0f / clip.w;
    float ndcX = clip.x * invW;
    float ndcY = clip.y * invW;
    float ndcZ = clip.z * invW;

    screen.x = (ndcX + 1.0f) * 0.5f * width;
    screen.y = (1.0f - ndcY) * 0.5f * height;
    screen.z = ndcZ;
    return true;
}