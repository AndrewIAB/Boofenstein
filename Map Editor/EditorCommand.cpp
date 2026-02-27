#include "EditorCommand.hpp"

#include <iostream>

void EditorCommand::CreatePointCommand::push(MapBuilder& map) {
	MapVec2* point_at = map.get_point_at(this->pos);
	
	if (point_at != nullptr) { return; }

	map.add_point(this->pos);
}

void EditorCommand::CreatePointCommand::pull(MapBuilder& map) {
	MapVec2* point_at = map.get_point_at(this->pos);

	if (point_at == nullptr) { return; }

	map.remove_point(point_at);
}

void EditorCommand::CreateSegmentCommand::push(MapBuilder& map) {
	MapVec2* point_at_a = map.get_point_at(this->point_pos_a);
	MapVec2* point_at_b = map.get_point_at(this->point_pos_b);

	if (point_at_a == nullptr) { point_at_a = std::addressof(map.add_point(this->point_pos_a)); }
	if (point_at_b == nullptr) { point_at_b = std::addressof(map.add_point(this->point_pos_b)); }

	map.add_segment(MapSegment(point_at_a, point_at_b));
}

void EditorCommand::CreateSegmentCommand::pull(MapBuilder& map) {
	MapVec2* point_at_a = map.get_point_at(this->point_pos_a);
	MapVec2* point_at_b = map.get_point_at(this->point_pos_b);

	if (point_at_a == nullptr || point_at_b == nullptr) { return; }
	
	MapSegment* segment_at = map.get_segment(point_at_a, point_at_b);

	if (segment_at == nullptr) { return; }

	map.remove_segment(segment_at);
}

void EditorCommand::DeletePointCommand::push(MapBuilder& map) {
	MapVec2* point_at = map.get_point_at(this->pos);

	if (point_at == nullptr) { return; }

	map.remove_point(point_at);
}

void EditorCommand::DeletePointCommand::pull(MapBuilder& map) {
	MapVec2* point_at = map.get_point_at(this->pos);

	if (point_at != nullptr) { return; }

	map.add_point(this->pos);
}

void EditorCommand::DeleteSegmentCommand::push(MapBuilder& map) {
	MapVec2* point_at_a = map.get_point_at(this->point_pos_a);
	MapVec2* point_at_b = map.get_point_at(this->point_pos_b);

	if (point_at_a == nullptr || point_at_b == nullptr) { return; }

	MapSegment* segment_at = map.get_segment(point_at_a, point_at_b);

	if (segment_at == nullptr) { return; }

	// Set these so when this command is pulled it doesn't create a segment with the wrong orientation.
	point_pos_a = *segment_at->point_a;
	point_pos_b = *segment_at->point_b;
	map.remove_segment(segment_at);
}

void EditorCommand::DeleteSegmentCommand::pull(MapBuilder& map) {
	MapVec2* point_at_a = map.get_point_at(this->point_pos_a);
	MapVec2* point_at_b = map.get_point_at(this->point_pos_b);

	if (point_at_a == nullptr) { point_at_a = std::addressof(map.add_point(this->point_pos_a)); }
	if (point_at_b == nullptr) { point_at_b = std::addressof(map.add_point(this->point_pos_b)); }

	map.add_segment(MapSegment(point_at_a, point_at_b));
}

void EditorCommand::MovePointCommand::push(MapBuilder& map) {
	MapVec2* point_at = map.get_point_at(this->prev_pos);

	if (point_at == nullptr) { return; }
	if (map.get_point_at(this->pos) != nullptr) { return; }

	*point_at = pos;
}

void EditorCommand::MovePointCommand::pull(MapBuilder& map) {
	MapVec2* point_at = map.get_point_at(this->pos);

	if (point_at == nullptr) { return; }
	if (map.get_point_at(this->prev_pos) != nullptr) { return; }

	*point_at = this->prev_pos;
}

void EditorCommand::ChangeMaterialCommand::push(MapBuilder& map) {
	MapVec2* point_at_a = map.get_point_at(this->point_pos_a);
	MapVec2* point_at_b = map.get_point_at(this->point_pos_b);
	
	if (point_at_a == nullptr || point_at_b == nullptr) { return; }

	MapSegment* segment_at = map.get_segment(point_at_a, point_at_b);

	if (segment_at == nullptr) { return; }

	segment_at->material_name = this->material;
}

void EditorCommand::ChangeMaterialCommand::pull(MapBuilder& map) {
	MapVec2* point_at_a = map.get_point_at(this->point_pos_a);
	MapVec2* point_at_b = map.get_point_at(this->point_pos_b);

	if (point_at_a == nullptr || point_at_b == nullptr) { return; }

	MapSegment* segment_at = map.get_segment(point_at_a, point_at_b);

	if (segment_at == nullptr) { return; }

	segment_at->material_name = this->prev_material;
}

