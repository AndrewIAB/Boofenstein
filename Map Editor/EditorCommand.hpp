#pragma once

#include <string>

#include "MapTypes.hpp"
#include "MapBuilder.hpp"

namespace EditorCommand {

	class EditorCommand {
	public:
		~EditorCommand() {}

		virtual std::string get_command()  = 0;
		virtual void push(MapBuilder& map) = 0;
		virtual void pull(MapBuilder& map) = 0;

		bool operator==(const EditorCommand& command) { return std::addressof(command) == this; }
		bool operator!=(const EditorCommand& command) { return std::addressof(command) != this; }
	};

	class EmptyCommand : public EditorCommand {
	public:
		EmptyCommand() {}

		std::string get_command() { return "Empty/Redundancy Command."; }
		void pull(MapBuilder& map) { return; }
		void push(MapBuilder& map) { return; }
	};

	class CreatePointCommand : public EditorCommand {
	public:
		MapVec2 pos;
		CreatePointCommand(MapVec2 pos) {
			this->pos = pos;
		}

		std::string get_command() { return "Create Point : " + pos.to_string(); }
		void push(MapBuilder& map);
		void pull(MapBuilder& map);
	};

	class CreateSegmentCommand : public EditorCommand {
	public:
		MapVec2 point_pos_a;
		MapVec2 point_pos_b;
		CreateSegmentCommand(MapVec2 point_pos_a, MapVec2 point_pos_b) {
			this->point_pos_a = point_pos_a;
			this->point_pos_b = point_pos_b;
		}

		std::string get_command() { return "Create Segment : " + point_pos_a.to_string() + " : " + point_pos_b.to_string(); }
		void push(MapBuilder& map);
		void pull(MapBuilder& map);
	};

	class DeletePointCommand : public EditorCommand {
	public:
		MapVec2 pos;
		DeletePointCommand(MapVec2 pos) {
			this->pos = pos;
		}

		std::string get_command() { return "Delete Point : " + pos.to_string(); }
		void push(MapBuilder& map);
		void pull(MapBuilder& map);
	};

	class DeleteSegmentCommand : public EditorCommand {
	public:
		MapVec2 point_pos_a;
		MapVec2 point_pos_b;
		DeleteSegmentCommand(MapVec2 point_pos_a, MapVec2 point_pos_b) {
			this->point_pos_a = point_pos_a;
			this->point_pos_b = point_pos_b;
		}

		std::string get_command() { return "Delete Segment : " + point_pos_a.to_string() + " : " + point_pos_b.to_string(); }
		void push(MapBuilder& map);
		void pull(MapBuilder& map);
	};

	class MovePointCommand : public EditorCommand {
	public:
		MapVec2 prev_pos;
		MapVec2 pos;
		MovePointCommand(MapVec2 prev_pos, MapVec2 pos) {
			this->prev_pos = prev_pos;
			this->pos = pos;
		}

		std::string get_command() { return "Move Point : FROM " + prev_pos.to_string() + " : TO " + pos.to_string(); }
		void push(MapBuilder& map);
		void pull(MapBuilder& map);
	};

	class ChangeMaterialCommand : public EditorCommand {
	public:
		MapVec2 point_pos_a;
		MapVec2 point_pos_b;
		std::string prev_material;
		std::string material;
		ChangeMaterialCommand(MapVec2 point_pos_a, MapVec2 point_pos_b, std::string prev_material, std::string material) {
			this->point_pos_a = point_pos_a;
			this->point_pos_b = point_pos_b;
			this->prev_material = prev_material;
			this->material = material;
		}

		std::string get_command() { return 
				"Change Material : " +
				point_pos_a.to_string() + " : " + point_pos_b.to_string() +
				" : PREV " + prev_material + " : NEW " + material; }

		void push(MapBuilder& map);
		void pull(MapBuilder& map);
	};
}