#include "MapBuilder.hpp"

#include <iostream>

static int next_id = 0;

MapBuilder::MapBuilder() {
	this->id = ++next_id;
}

MapVec2& MapBuilder::add_point(MapVec2 pos) {
	points.push_back(pos);
	return points.back();
}

MapSegment& MapBuilder::add_segment(MapSegment segment) {
	segments.push_back(segment);
	return segments.back();
}

void MapBuilder::remove_point(MapVec2* ptr) {
	//for (std::list<MapSegment>::iterator it = segments.begin(); it != segments.end(); ++it) {
	//	if (it->point_a == ptr || it->point_b == ptr) {
	//		remove_segment(std::addressof(*it));
	//	}
	//}
	points.remove_if([ptr](MapVec2& v) { return *ptr == v; });
}

void MapBuilder::remove_segment(MapSegment* ptr) {
	segments.remove_if([ptr](MapSegment& s) { return *ptr == s; });
}

MapVec2* MapBuilder::get_point_at(MapVec2 pos) {
	for (std::list<MapVec2>::iterator it = points.begin(); it != points.end(); ++it) {
		if (it->x == pos.x && it->y == pos.y) {
			return std::addressof(*it);
		}
	}

	return nullptr;
}

MapSegment* MapBuilder::get_segment(MapVec2* point_a, MapVec2* point_b) {
	for (std::list<MapSegment>::iterator it = segments.begin(); it != segments.end(); ++it) {
		if (
			(it->point_a == point_a && it->point_b == point_b) ||
			(it->point_a == point_b && it->point_b == point_a)
		) {
			return std::addressof(*it);
		}
	}

	return nullptr;
}