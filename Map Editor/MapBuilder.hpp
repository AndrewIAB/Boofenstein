#pragma once

#include <string>
#include <vector>
#include <list>
#include <cstdint>

#include "MapTypes.hpp"
#include "Vec_n.hpp"

#define DEFAULT_MAP_UNIT_SIZE 16

#define MAP_MIN_POS		INT16_MIN
#define MAP_MAX_POS		INT16_MAX

class MapBuilder {
private:
	MapPositionComponent		unit_size		= DEFAULT_MAP_UNIT_SIZE;

	std::list<MapVec2>			points;
	std::list<MapSegment>		segments;

	MapPositionComponent		roof_height		= unit_size * 2;

	int id;
public:
	MapBuilder();
	~MapBuilder() {}

	int get_id() { return id; }
	bool operator==(MapBuilder& m) { return m.get_id() == id; }

	MapVec2& add_point(MapVec2 pos);
	MapSegment& add_segment(MapSegment segment);

	void remove_point(MapVec2* ptr);
	void remove_segment(MapSegment* ptr);

	MapVec2* get_point_at(MapVec2 pos);
	MapSegment* get_segment(MapVec2* point_a, MapVec2* point_b);

	inline std::list<MapVec2>& get_point_list() { return points; }
	inline std::list<MapSegment>& get_segment_list() { return segments; }

	inline Vec2 map_position_to_gl(const MapVec2& v) { return Vec2(v.x, v.y) / (float)unit_size; }
	inline float map_position_to_gl(float a) { return a / (float)unit_size; }

	inline void set_unit_size(MapPositionComponent a) { unit_size = a; }
	inline MapPositionComponent get_unit_size() { return unit_size; }

	inline void set_roof_height(MapPositionComponent a) { roof_height = a; }
	inline MapPositionComponent get_roof_height() { return roof_height; }
};