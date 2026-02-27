#pragma once

typedef int16_t		MapPositionComponent;

struct MapMaterial {
	std::string path;
};

struct MapVec2 {
	MapPositionComponent x, y;

	inline bool operator==(MapVec2& v) { return x == v.x && y == v.y; }
	inline bool operator!=(MapVec2& v) { return x != v.x && y != v.y; }

	inline MapVec2() { this->x = 0; this->y = 0; }
	inline MapVec2(MapPositionComponent x) { this->x = x; this->y = x; }
	inline MapVec2(MapPositionComponent x, MapPositionComponent y) { this->x = x; this->y = y; }

	inline std::string to_string() { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }

	inline MapVec2 operator+(const MapVec2& v) { return MapVec2(x + v.x, y + v.y); }
	inline MapVec2 operator-(const MapVec2& v) { return MapVec2(x - v.x, y - v.y); }
	inline MapVec2 operator*(MapPositionComponent a) { return MapVec2(x * a, y * a); }
	inline MapVec2 operator/(MapPositionComponent a) { return MapVec2(x / a, y / a); }
};

struct MapSegment {
	MapVec2* point_a;
	MapVec2* point_b;

	std::string material_name = "";

	inline MapSegment(MapVec2* a, MapVec2* b, std::string material_name = "") { point_a = a; point_b = b; this->material_name = material_name; }

	inline bool operator==(MapSegment& s) { return s.point_a == point_a && s.point_b == point_b; }
};