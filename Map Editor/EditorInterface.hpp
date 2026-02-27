#pragma once

#include "EditorMap.hpp"

namespace EditorInterface {
	enum {
		TOOL_Select,
		TOOL_Move,
		TOOL_CreatePoints,
		TOOL_CreateSegments,
		TOOL_DeletePoints,
		TOOL_DeleteSegments,
		TOOL_DrawSegments,
		TOOL_SplitSegments
	};

	enum {
		VIEW_2D,
		VIEW_3D
	};

	struct EditorState {
		struct {
			int line_width = 2;
			int point_size = 4;
			Vec3 sel_point_color = Vec3(1, 0, 0);
			Vec3 line_color = Vec3(1, 1, 1);

			bool draw_grid = true;
		} preferences;

		struct {
			int current_tool = TOOL_Select;

			MapVec2 mouse_pos_map;
			bool mouse_clicked = false;
			bool mouse_released = false;
			bool mouse_down = false;
		} input;

		int view_mode = VIEW_2D;
		bool in_interface = false;

		MapVec2 offset = MapVec2();
		MapPositionComponent scale = DEFAULT_MAP_UNIT_SIZE;

		MapVec2* sel_point = nullptr;
		MapSegment* sel_segment = nullptr;

		EditorMap* map = nullptr;
	};
	
	void init();
	void quit();

	void update();

	EditorState& get_editor_state();
}