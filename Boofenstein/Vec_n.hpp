#pragma once

#include <cmath>

class Vec2 {
public:
	float x = 0, y = 0;

	inline Vec2(float x, float y) { this->x = x; this->y = y; }
	inline Vec2() {}

	inline float length() { return sqrt(x * x + y * y); }
	inline void normalize() { operator/=(length()); }

	inline Vec2 operator-(const Vec2& v) { return Vec2(x - v.x, y - v.y); }
	inline Vec2 operator*(float a) { return Vec2(x * a, y * a); }
	inline void operator/=(float a) { x /= a; y /= a; }
};

class Vec3 {
public:
	float x = 0, y = 0, z = 0;

	inline Vec3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	inline Vec3() {}

	inline void operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; }
	inline void operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; }
	inline Vec3 operator*(float a) { return Vec3(x * a, y * a, z * a); }
};

class Vec4 {
public:
	float x = 0, y = 0, z = 0, w = 0;

	inline Vec4(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	inline Vec4() {}
};