#pragma once

#include <cmath>

class Vec2 {
public:
	float x = 0, y = 0;

	inline Vec2(float x, float y) { this->x = x; this->y = y; }
	inline Vec2() {}

	inline void to_array(float* out) { out[0] = x; out[1] = y; }
	inline void from_array(float* out) { x = out[0]; y = out[1]; }

	inline float length() { return sqrt(x * x + y * y); }
	inline void normalize() { operator/=(length()); }

	inline Vec2 operator+(const Vec2& v) { return Vec2(x + v.x, y + v.y); }
	inline Vec2 operator-(const Vec2& v) { return Vec2(x - v.x, y - v.y); }
	inline Vec2 operator*(float a) { return Vec2(x * a, y * a); }
	inline Vec2 operator/(Vec2 v) { return Vec2(x / v.x, y / v.y); }
	inline Vec2 operator/(float a) { return Vec2(x / a, y / a); }

	inline void operator-=(float a) { x -= a; y -= a; }
	inline void operator+=(float a) { x += a; y += a; }
	inline void operator*=(float a) { x *= a; y *= a; }
	inline void operator/=(float a) { x /= a; y /= a; }
};

class Vec3 {
public:
	float x = 0, y = 0, z = 0;

	inline Vec3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	inline Vec3() {}

	inline void to_array(float* out) { out[0] = x; out[1] = y; out[2] = z; }
	inline void from_array(float* out) { x = out[0]; y = out[1]; z = out[2]; }

	inline void operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; }
	inline void operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; }
	inline Vec3 operator*(float a) { return Vec3(x * a, y * a, z * a); }

	inline float dot(const Vec3& v) { return x * v.x + y * v.y + z * v.z; }
};

class Vec4 {
public:
	float x = 0, y = 0, z = 0, w = 0;

	inline Vec4(float x, float y, float z, float w) { this->x = x; this->y = y; this->z = z; this->w = w; }
	inline Vec4() {}

	inline void to_array(float* out) { out[0] = x; out[1] = y; out[2] = z; out[3] = w; }
	inline void from_array(float* out) { x = out[0]; y = out[1]; z = out[2]; w = out[3]; }

	inline float dot(const Vec4& v) { return x * v.x + y * v.y + z * v.z + w * v.w; }
};

//class Mat3 {
//public:
//	Vec3 c1, c2, c3;
//
//	inline Mat3(Vec3 v1, Vec3 v2, Vec3 v3) { c1 = v1; c2 = v2; c3 = v3; }
//	inline Mat3() {}
//
//	static Mat3 identity() { return Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)); }
//
//	inline Vec3 get_r1() { return Vec3(c1.x, c2.x, c3.x); }
//	inline Vec3 get_r2() { return Vec3(c1.y, c2.y, c3.y); }
//	inline Vec3 get_r3() { return Vec3(c1.z, c2.z, c3.z); }
//
//	inline Mat3 operator*(const Mat3& m) {
//		Mat3 o;
//
//		Vec3 r1 = get_r1();
//		Vec3 r2 = get_r2();
//		Vec3 r3 = get_r3();
//
//		o.c1.x = r1.dot(m.c1);	o.c1.y = r1.dot(m.c2);	o.c1.z = r1.dot(m.c3);
//		o.c2.x = r2.dot(m.c1);	o.c2.y = r2.dot(m.c2);	o.c2.z = r2.dot(m.c3);
//		o.c3.x = r3.dot(m.c1);	o.c3.y = r3.dot(m.c2);	o.c3.z = r3.dot(m.c3);
//
//		return o;
//	}
//};

//class Mat4 {
//public:
//	Vec4 c1, c2, c3, c4;
//
//	inline Mat4(Vec4 v1, Vec4 v2, Vec4 v3, Vec4 v4) { c1 = v1; c2 = v2; c3 = v3; c4 = v4; }
//	inline Mat4() {}
//
//	static Mat4 identity() { return Mat4(Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1)); }
//
//	inline Vec4 get_r1() { return Vec4(c1.x, c2.x, c3.x, c4.x); }
//	inline Vec4 get_r2() { return Vec4(c1.y, c2.y, c3.y, c4.y); }
//	inline Vec4 get_r3() { return Vec4(c1.z, c2.z, c3.z, c4.z); }
//	inline Vec4 get_r4() { return Vec4(c1.w, c2.w, c3.w, c4.w); }
//
//	inline Mat4 operator*(const Mat4& m) {
//		Mat4 o;
//
//		Vec4 r1 = get_r1();
//		Vec4 r2 = get_r2();
//		Vec4 r3 = get_r3();
//		Vec4 r4 = get_r4();
//
//		o.c1.x = r1.dot(m.c1);	o.c1.y = r1.dot(m.c2);	o.c1.z = r1.dot(m.c3);	o.c1.w = r1.dot(m.c4);
//		o.c2.x = r2.dot(m.c1);	o.c2.y = r2.dot(m.c2);	o.c2.z = r2.dot(m.c3);	o.c2.w = r2.dot(m.c4);
//		o.c3.x = r3.dot(m.c1);	o.c3.y = r3.dot(m.c2);	o.c3.z = r3.dot(m.c3);	o.c3.w = r3.dot(m.c4);
//		o.c4.x = r4.dot(m.c1);	o.c4.y = r4.dot(m.c2);	o.c4.z = r4.dot(m.c3);	o.c4.w = r4.dot(m.c4);
//
//		return o;
//	}
//};

class Mat3 {
public:
	Vec3 r1, r2, r3;

	inline Mat3(Vec3 v1, Vec3 v2, Vec3 v3) { r1 = v1; r2 = v2; r3 = v3; }
	inline Mat3() {}

	static Mat3 identity() { return Mat3(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)); }
	static Mat3 translate(Vec2 v) { return Mat3(Vec3(1, 0, v.x), Vec3(0, 1, v.y), Vec3(0, 0, 1)); }
	static Mat3 scale(Vec2 v) { return Mat3(Vec3(v.x, 0, 0), Vec3(0, v.y, 0), Vec3(0, 0, 1)); }
	static Mat3 scale(float a) { return Mat3(Vec3(a, 0, 0), Vec3(0, a, 0), Vec3(0, 0, 1)); }

	inline Vec3 get_c1() const { return Vec3(r1.x, r2.x, r3.x); }
	inline Vec3 get_c2() const { return Vec3(r1.y, r2.y, r3.y); }
	inline Vec3 get_c3() const { return Vec3(r1.z, r2.z, r3.z); }

	inline Mat3 operator*(const Mat3& m) {
		Mat3 o;

		Vec3 c1 = m.get_c1();
		Vec3 c2 = m.get_c2();
		Vec3 c3 = m.get_c3();

		o.r1.x = r1.dot(c1);	o.r1.y = r1.dot(c2);	o.r1.z = r1.dot(c3);
		o.r2.x = r2.dot(c1);	o.r2.y = r2.dot(c2);	o.r2.z = r2.dot(c3);
		o.r3.x = r3.dot(c1);	o.r3.y = r3.dot(c2);	o.r3.z = r3.dot(c3);

		return o;
	}
};

class Mat4 {
public:
	Vec4 r1, r2, r3, r4;

	inline Mat4(Vec4 v1, Vec4 v2, Vec4 v3, Vec4 v4) { r1 = v1; r2 = v2; r3 = v3; r4 = v4; }
	inline Mat4() {}

	static Mat4 identity() { return Mat4(Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1)); }
	static Mat4 translate(Vec3 v) { return Mat4(Vec4(1, 0, 0, v.x), Vec4(0, 1, 0, v.y), Vec4(0, 0, 1, v.z), Vec4(0, 0, 0, 1)); }
	static Mat4 scale(Vec3 v) { return Mat4(Vec4(v.x, 0, 0, 0), Vec4(0, v.y, 0, 0), Vec4(0, 0, v.z, 0), Vec4(0, 0, 0, 1)); }
	static Mat4 scale(float a) { return Mat4(Vec4(a, 0, 0, 0), Vec4(0, a, 0, 0), Vec4(0, 0, a, 0), Vec4(0, 0, 0, 1)); }

	inline Vec4 get_c1() const { return Vec4(r1.x, r2.x, r3.x, r4.x); }
	inline Vec4 get_c2() const { return Vec4(r1.y, r2.y, r3.y, r4.y); }
	inline Vec4 get_c3() const { return Vec4(r1.z, r2.z, r3.z, r4.z); }
	inline Vec4 get_c4() const { return Vec4(r1.w, r2.w, r3.w, r4.w); }

	inline Mat4 operator*(const Mat4& m) {
		Mat4 o;

		Vec4 c1 = m.get_c1();
		Vec4 c2 = m.get_c2();
		Vec4 c3 = m.get_c3();
		Vec4 c4 = m.get_c4();

		o.r1.x = r1.dot(c1);	o.r1.y = r1.dot(c2);	o.r1.z = r1.dot(c3);	o.r1.w = r1.dot(c4);
		o.r2.x = r2.dot(c1);	o.r2.y = r2.dot(c2);	o.r2.z = r2.dot(c3);	o.r2.w = r2.dot(c4);
		o.r3.x = r3.dot(c1);	o.r3.y = r3.dot(c2);	o.r3.z = r3.dot(c3);	o.r3.w = r3.dot(c4);
		o.r4.x = r4.dot(c1);	o.r4.y = r4.dot(c2);	o.r4.z = r4.dot(c3);	o.r4.w = r4.dot(c4);

		return o;
	}
};