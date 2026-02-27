#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "MapBuilder.hpp"
#include "EditorCommand.hpp"

typedef EditorCommand::EditorCommand EditorCommandBase;

class EditorMap {
private:
	MapBuilder map_builder;

	std::string title;
	bool changed = false;

	std::string map_path;
	std::string materials_path;

	std::list<EditorCommandBase*> command_log;
	std::list<EditorCommandBase*>::iterator command_iterator = command_log.begin();

	void remove_after_iterator();
public:
	EditorMap(std::string title) {
		this->title = title;
		// For redundancy. Probably not a good way of handling this lol.
		push_command(EditorCommand::EmptyCommand());
	}
	~EditorMap() {
		std::cout << "Closing Map : " << this->title << " :  " << command_log.size() << '\n';
		while (command_log.size() > 0) {
			std::cout << "Deleting command " << command_log.back() << '\n';
			delete command_log.back();
			command_log.pop_back();
		}
	}

	void push_command(const EditorCommand::EmptyCommand& command);
	void push_command(const EditorCommand::CreatePointCommand& command);
	void push_command(const EditorCommand::CreateSegmentCommand& command);
	void push_command(const EditorCommand::DeletePointCommand& command);
	void push_command(const EditorCommand::DeleteSegmentCommand& command);
	void push_command(const EditorCommand::MovePointCommand& command);
	void push_command(const EditorCommand::ChangeMaterialCommand& command);
	void roll_back_command();
	void roll_forward_command();

	inline MapVec2* get_point_at(MapVec2 pos) { return map_builder.get_point_at(pos); }
	inline MapSegment* get_segment(MapVec2* point_a, MapVec2* point_b) { return map_builder.get_segment(point_a, point_b); }

	inline MapVec2& get_last_point() { return map_builder.get_point_list().back(); }
	inline MapSegment& get_last_segment() { return map_builder.get_segment_list().back(); }

	inline void remove_associated_segments(MapVec2* point) {
		std::list<MapSegment>& segments = map_builder.get_segment_list();
		std::list<MapSegment>::iterator it = segments.begin();
		while (it != segments.end()) {
			if (it->point_a != point && it->point_b != point) { it++; continue; }

			std::list<MapSegment>::iterator curr_it = it;
			it++;

			push_command(EditorCommand::DeleteSegmentCommand(*curr_it->point_a, *curr_it->point_b));
		}
	}

	inline int get_map_id() { return map_builder.get_id(); }

	inline const MapBuilder& get_map() const { return this->map_builder; }

	inline const std::string& get_title()	{ return this->title; }
	inline bool get_changed()				{ return this->changed; }

	inline void set_title(const std::string& title)	{ this->title = title; }
	inline void set_changed(bool changed)			{ this->changed = changed; }

	inline const std::string& get_map_path()		{ return this->map_path; }
	inline const std::string& get_materials_path()	{ return this->materials_path; }
	
	inline void set_map_path(const std::string& path)		{ this->map_path = path; }
	inline void set_materials_path(const std::string& path)	{ this->materials_path = path; }

	inline int get_unit_size() { return map_builder.get_unit_size(); }
	inline MapPositionComponent get_roof_height() { return map_builder.get_roof_height(); }

	inline void set_unit_size(int unit_size) { map_builder.set_unit_size(unit_size); }
	inline void set_roof_height(MapPositionComponent height) { map_builder.set_roof_height(height); }
};


inline bool operator==(EditorMap& ref_a, EditorMap& ref_b) { return ref_a.get_map_id() == ref_b.get_map_id(); }
inline bool operator!=(EditorMap& ref_a, EditorMap& ref_b) { return ref_a.get_map_id() != ref_b.get_map_id(); }

inline bool operator==(const EditorMap& map1, const EditorMap& map2) {
	return std::addressof(map1) == std::addressof(map2);
}