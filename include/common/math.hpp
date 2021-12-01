#pragma once

struct Math
{
    static constexpr float PI = 3.141592653f;

    template<typename T>
    static T sign(T v)
    {
        return v < 0.0f ? -1.0f : 1.0f;
    }
    
    static float sigm(float x)
    {
        return 1.0f / (1.0f + exp(-x));
    }
    
    static float sigm_0(float x)
    {
        return sigm(x) - 0.5f;
    }

    static float radToDeg(float r)
    {
        constexpr float radian_to_deg = 180.0f / PI;
        return r * radian_to_deg;
    }
};


struct MathVec2
{
    template<template<typename> class Vec2Type, typename T>
    static T length(Vec2Type<T> v)
    {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    template<template<typename> class Vec2Type, typename T>
    static T angle(Vec2Type<T> v_1)
    {
        return -atan2(-v_1.y, v_1.x);
    }

    template<template<typename> class Vec2Type, typename T>
    static T angle(Vec2Type<T> v_1, Vec2Type<T> v_2)
    {
        const T dot = v_1.x * v_2.x + v_1.y * v_2.y;
        const T det = v_1.x * v_2.y - v_1.y * v_2.x;
        return atan2(det, dot);
    }
    
    template<template<typename> class Vec2Type, typename T>
    static T dot(Vec2Type<T> v1, Vec2Type<T> v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    template<template<typename> class Vec2Type, typename T>
    static T cross(Vec2Type<T> v1, Vec2Type<T> v2)
    {
        return v1.x * v2.y - v1.y * v2.x;
    }
    
    template<typename Vec2Type>
    static Vec2Type normal(const Vec2Type& v)
    {
        return {-v.y, v.x};
    }

    template<typename Vec2Type>
    static Vec2Type rotate(const Vec2Type& v, float angle)
    {
        const float ca = cos(angle);
        const float sa = sin(angle);
        return {ca * v.x - sa * v.y, sa * v.x + ca * v.y};
    }

    template<typename Vec2Type>
    static Vec2Type rotateDir(const Vec2Type& v, const Vec2Type& dir)
    {
        return { dir.x * v.x - dir.y * v.y, dir.y * v.x + dir.x * v.y };
    }

    template<typename Vec2Type>
    static Vec2Type normalize(const Vec2Type& v)
    {
        return v / length(v);
    }

    template<template<typename> class Vec2Type, typename T>
    static Vec2Type<T> reflect(const Vec2Type<T>& v, const Vec2Type<T>& n)
    {
        return v - n * (MathVec2::dot(v, n) * 2.0f);
    }
};

