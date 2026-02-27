#include <iostream>

#include "Window.hpp"
#include "RendererBase.hpp"
#include "EditorRenderer.hpp"
#include "EditorInterface.hpp"
#include "Vec_n.hpp"

void main() {
    Window::init(500, 500, "Window", false);
    Window::clear_color(0, 0, 0);

    EditorRenderer renderer;

    renderer.set_orthograpic(1.0f / 10.0f, 1.0f);
    renderer.set_perspective(60, 100.0, 0.1, 1.0f);

    Vec2 offset = Vec2(0.5, -1);
    float rotation = 0;

    EditorInterface::init();

    while (Window::poll()) {
        Window::clear();

        renderer.set_view_offset(offset, 0.5);
        renderer.set_view_rotation(rotation);
        renderer.set_view_transform();

        rotation += 0.0001;

        renderer.draw_test();

        EditorInterface::update();

        Window::swap();
    }

    EditorInterface::quit();
    Window::quit();
}

/*
#include <iostream>
#include <list>

#include "Window.hpp"
#include "EditorInterface.hpp"
#include "EditorRenderer.hpp"

typedef EditorInterface::EditorState EditorState;

static void draw_map(const EditorState& editor_state) {
    if (editor_state.view_mode == EditorInterface::VIEW_2D) {
        EditorRenderer::set_2d_view(editor_state.map->get_map(), editor_state.offset, editor_state.scale);

        EditorRenderer::draw_2D(
            editor_state.map->get_map(), editor_state.sel_point, editor_state.sel_segment, editor_state.preferences.draw_grid,
            editor_state.preferences.sel_point_color, editor_state.preferences.line_color,
            1.0, 2.0, editor_state.preferences.line_width, editor_state.preferences.point_size
        );
    }
}

static MapVec2* create_select_point(EditorState& editor_state, MapVec2* point_at) {
    if (point_at != nullptr) { return point_at; }

    editor_state.map->push_command(EditorCommand::CreatePointCommand(editor_state.input.mouse_pos_map));
    
    return std::addressof(editor_state.map->get_last_point());
}

static void use_tool_select(EditorState& editor_state, MapVec2* point_at) {
    if (editor_state.input.mouse_clicked == false) { return; }
    // If pointing at nothing, clear selection
    if (point_at == nullptr) {
        editor_state.sel_point = nullptr;
        editor_state.sel_segment = nullptr;
        return;
    }

    // Select point
    if (editor_state.sel_point == nullptr) {
        editor_state.sel_point = point_at;
        editor_state.sel_segment = nullptr;
    }
    // Select segment
    else  {
        editor_state.sel_segment = editor_state.map->get_segment(editor_state.sel_point, point_at);
        // Need to reset after sel_segment since sel_segment uses the last sel_point
        editor_state.sel_point = nullptr;
    }
}

static void use_tool_move(EditorState& editor_state, MapVec2* point_at) {
    static MapVec2 last_pos;

    // Select point to move on mouse click
    if (editor_state.input.mouse_clicked && point_at != nullptr) {
        editor_state.sel_point = point_at;
        last_pos = *point_at;
    }
    // Move to mouse location on mouse release
    else if (editor_state.input.mouse_released && editor_state.sel_point != nullptr) {
        if (point_at == nullptr)
            editor_state.map->push_command(EditorCommand::MovePointCommand(last_pos, editor_state.input.mouse_pos_map));
        
        editor_state.sel_point = nullptr;
    }
}

static void use_tool_create_points(EditorState& editor_state, MapVec2* point_at) {
    if (editor_state.input.mouse_clicked == false) { return; }
    if (point_at != nullptr) { return; }

    //editor_state.map->push_command(EditorCommand::CreatePointCommand(editor_state.input.mouse_pos_map));
    //editor_state.sel_point = editor_state.map->get_map().get_point_at(editor_state.input.mouse_pos_map);
    editor_state.sel_point = create_select_point(editor_state, point_at);
}

static void use_tool_create_segments(EditorState& editor_state, MapVec2* point_at) {
    if (editor_state.input.mouse_clicked == false) { return; }
    if (point_at == nullptr) {
        editor_state.sel_point = nullptr;
        editor_state.sel_segment = nullptr;
        return;
    }
    if (editor_state.sel_point == nullptr) {
        editor_state.sel_point = point_at;
        editor_state.sel_segment = nullptr;
        return;
    }
    if (point_at == nullptr) { return; }

    // Do not create segment on top of existing segment
    // Just delete old segment and create where it was
    // Good for flipping segment normal
    if (editor_state.map->get_segment(editor_state.sel_point, point_at) != nullptr) {
        editor_state.map->push_command(EditorCommand::DeleteSegmentCommand(*editor_state.sel_point, *point_at));
    }

    // Create segment and highlight it.
    editor_state.map->push_command(EditorCommand::CreateSegmentCommand(*editor_state.sel_point, *point_at));
    editor_state.sel_point = nullptr;
    editor_state.sel_segment = std::addressof(editor_state.map->get_last_segment());
}

static void use_tool_delete_points(EditorState& editor_state, MapVec2* point_at) {
    if (editor_state.input.mouse_clicked == false) { return; }
    if (point_at == nullptr) { return; }

    editor_state.map->remove_associated_segments(point_at);

    editor_state.map->push_command(EditorCommand::DeletePointCommand(*point_at));
}

static void use_tool_delete_segment(EditorState& editor_state, MapVec2* point_at) {
    if (editor_state.input.mouse_clicked == false) { return; }
    if (point_at == nullptr) {
        editor_state.sel_point = nullptr;
        editor_state.sel_segment = nullptr;
        return;
    }
    if (editor_state.sel_point == nullptr) {
        editor_state.sel_point = point_at;
        editor_state.sel_segment = nullptr;
        return;
    }

    // Delete segment and clear selection
    editor_state.map->push_command(EditorCommand::DeleteSegmentCommand(*editor_state.sel_point, *point_at));
    editor_state.sel_point = nullptr;
    editor_state.sel_segment = nullptr;
}

static void use_tool_draw_segments(EditorState& editor_state, MapVec2* point_at) {
    if (editor_state.input.mouse_clicked == false) { return; }
    if (editor_state.sel_point == nullptr) {
        editor_state.sel_point = create_select_point(editor_state, point_at);
        return;
    }

    MapVec2* point_b = create_select_point(editor_state, point_at);

    // Do not draw over existing segments.
    if (editor_state.map->get_segment(editor_state.sel_point, point_b) != nullptr) { return; }

    editor_state.map->push_command(EditorCommand::CreateSegmentCommand(*editor_state.sel_point, *point_b));

    // Treat existing point as endpoint and dont select.
    if (point_at == nullptr) {
        editor_state.sel_point = point_b;
    }
    else {
        editor_state.sel_point = nullptr;
    }
}

static void use_current_tool(EditorState& editor_state) {
    if (
        editor_state.input.mouse_clicked == false &&
        editor_state.input.mouse_released == false &&
        editor_state.input.mouse_down == false
    ) { return; }

    MapVec2* point_at = editor_state.map->get_point_at(editor_state.input.mouse_pos_map);

    switch (editor_state.input.current_tool) {
    case EditorInterface::TOOL_Select:
        use_tool_select(editor_state, point_at);
        break;
    case EditorInterface::TOOL_Move:
        use_tool_move(editor_state, point_at);
        break;
    case EditorInterface::TOOL_CreatePoints:
        use_tool_create_points(editor_state, point_at);
        break;
    case EditorInterface::TOOL_CreateSegments:
        use_tool_create_segments(editor_state, point_at);
        break;
    case EditorInterface::TOOL_DeletePoints:
        use_tool_delete_points(editor_state, point_at);
        break;
    case EditorInterface::TOOL_DeleteSegments:
        use_tool_delete_segment(editor_state, point_at);
        break;
    case EditorInterface::TOOL_DrawSegments:
        use_tool_draw_segments(editor_state, point_at);
        break;
    case EditorInterface::TOOL_SplitSegments:
        break;
    }
}

int main() {
    Window::init(600, 600, "Map Editor", true);
    Window::clear_color(0, 0, 0);

    EditorInterface::init();
    EditorRenderer::init();

    EditorState& editor_state = EditorInterface::get_editor_state();

    while (Window::poll()) {
        Window::clear();

        if (editor_state.map != nullptr) {
            if (editor_state.view_mode == EditorInterface::VIEW_2D && !editor_state.in_interface) {
                use_current_tool(editor_state);
            }

            draw_map(editor_state);
        }

        EditorInterface::update();

        Window::swap();
    }

    EditorRenderer::quit();
    EditorInterface::quit();
    Window::quit();
    return 0;
}
*/