#include "EditorInterface.hpp"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui.h>

#include "Window.hpp"

typedef EditorInterface::EditorState EditorState;

#define MIN(x, y) ( x < y ? x : y )
#define MAX(x, y) ( x > y ? x : y )

#define IS_EDITING_TEST() editor_state.in_interface = editor_state.in_interface || (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())

static EditorState editor_state;

static bool editor_tools_open = false;
static bool properties_open = false;
static bool map_materials_open = false;
static bool editor_preferences_open = false;

static bool new_map_fail_popup = false;
static bool close_map_fail_popup = false;
static bool unsaved_work_popup = false;

static EditorMap* map = nullptr;

void EditorInterface::init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.LogFilename = NULL;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Window::get_window_ptr(), true);
    ImGui_ImplOpenGL3_Init();
}

void EditorInterface::quit() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

static void new_map(std::string title = "Untitled") {
    if (map != nullptr) {
        new_map_fail_popup = true;
        return;
    }
    
    map = new EditorMap(title);
}

static void close_map() {
    if (map == nullptr) {
        close_map_fail_popup = true;
        return;
    }
    if (map->get_changed()) {
        unsaved_work_popup = true;
        return;
    }

    delete map;
    map = nullptr;
}

static void undo_command() {
    editor_state.sel_point = nullptr;
    editor_state.sel_segment = nullptr;

    map->roll_back_command();
}

static void redo_command() {
    editor_state.sel_point = nullptr;
    editor_state.sel_segment = nullptr;

    map->roll_forward_command();
}

static void show_menu() {
    bool map_is_open = map != nullptr;

    if (ImGui::BeginMenu("File")) {
        IS_EDITING_TEST();

        if (ImGui::MenuItem("New", "Ctrl + T")) {
            new_map();
        }
        if (ImGui::MenuItem("Open", "")) {
            // TODO: FILE OPEN
        }
        if (ImGui::MenuItem("Close", "Ctrl + W", nullptr, map_is_open)) {
            close_map();
        }
        
        ImGui::Separator();

        if (ImGui::MenuItem("Save", "Ctrl + S", nullptr, map_is_open)) {
            // TODO: SAVE
        }
        if (ImGui::MenuItem("Save As", "Ctrl + Shift + S", nullptr, map_is_open)) {
            // TODO: SAVE
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Undo", "Ctrl + Z", nullptr, map_is_open)) {
            undo_command();
        }

        if (ImGui::MenuItem("Redo", "Ctrl + Shift + Z", nullptr, map_is_open)) {
            redo_command();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window")) {
        IS_EDITING_TEST();

        ImGui::MenuItem("Editor Tools",     "Ctrl + Alt + T",     &editor_tools_open,    map_is_open);
        ImGui::MenuItem("Properties",       "Ctrl + Alt + P",     &properties_open,      map_is_open);
        ImGui::MenuItem("Map Materials",    "Ctrl + Alt + M",     &map_materials_open,   map_is_open);

        ImGui::MenuItem("Preferences", "", &editor_preferences_open);

        ImGui::EndMenu();
    }

    ImGui::SetNextItemWidth(100);
    ImGui::Combo("##view", &editor_state.view_mode, "2D View\0""3D View\0");

    if (map == nullptr) {
        ImGui::Text("No Map Opened");
    } else {
        std::string text = "Map: " + map->get_title();
        if (map->get_changed() == true) {
            text += " (*)";
        }
    
        ImGui::Text(text.data());
    }
}

static void show_tools_window() {
    if (ImGui::Begin("Editor Tools", &editor_tools_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        IS_EDITING_TEST();

        bool tool_changed = false;

        tool_changed |= ImGui::RadioButton("Select",            &editor_state.input.current_tool, EditorInterface::TOOL_Select);
        ImGui::SameLine();
        tool_changed |= ImGui::RadioButton("Move",              &editor_state.input.current_tool, EditorInterface::TOOL_Move);

        tool_changed |= ImGui::RadioButton("Create Points",     &editor_state.input.current_tool, EditorInterface::TOOL_CreatePoints);
        ImGui::SameLine();
        tool_changed |= ImGui::RadioButton("Create Segments",   &editor_state.input.current_tool, EditorInterface::TOOL_CreateSegments);

        tool_changed |= ImGui::RadioButton("Delete Points",     &editor_state.input.current_tool, EditorInterface::TOOL_DeletePoints);
        ImGui::SameLine();
        tool_changed |= ImGui::RadioButton("Delete Segments",   &editor_state.input.current_tool, EditorInterface::TOOL_DeleteSegments);

        tool_changed |= ImGui::RadioButton("Draw Segments",     &editor_state.input.current_tool, EditorInterface::TOOL_DrawSegments);
        ImGui::SameLine();
        tool_changed |= ImGui::RadioButton("Split Segments",    &editor_state.input.current_tool, EditorInterface::TOOL_SplitSegments);

        if (tool_changed) {
            editor_state.sel_point = nullptr;
            editor_state.sel_segment = nullptr;
        }
    }

    ImGui::End();
}

static void show_map_properties() {
    ImGui::Text("Map Properties");
    ImGui::Separator();

    std::string title = map->get_title();
    int unit_size = map->get_unit_size();
    int roof_height = map->get_roof_height();

    std::string map_path = map->get_map_path();
    std::string materials_path = map->get_materials_path();

    ImGui::InputText("Map Title", &title);

    ImGui::InputText("Map Path", &map_path);
    ImGui::InputText("Materials Path", &materials_path);

    ImGui::SliderInt("Unit Size", &unit_size, 1, DEFAULT_MAP_UNIT_SIZE);

    ImGui::InputInt("Roof Height", &roof_height, 1, unit_size);
    roof_height = MAX(roof_height, 1);

    map->set_title(title);
    map->set_unit_size(unit_size);
    map->set_roof_height(roof_height);

    map->set_map_path(map_path);
    map->set_materials_path(materials_path);
}

static void show_point_properties(MapVec2& sel_point) {
    int x = sel_point.x;
    int y = sel_point.y;

    ImGui::Text("Position");

    ImGui::InputInt("X", &x);
    ImGui::SameLine();
    ImGui::InputInt("Y", &y);

    if (x != sel_point.x || y != sel_point.y) {
        map->push_command(EditorCommand::MovePointCommand(sel_point, MapVec2(x, y)));
    }
}

static void show_segment_properties(MapSegment& sel_segment) {
    ImGui::InputText("Material Name", &sel_segment.material_name);

    ImGui::PushID(0);
    show_point_properties(*sel_segment.point_a);
    ImGui::PopID();
    ImGui::PushID(1);
    show_point_properties(*sel_segment.point_b);
    ImGui::PopID();
}

static void show_properties_window() {
    ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Once);
    if (ImGui::Begin("Properties", &properties_open, ImGuiWindowFlags_AlwaysAutoResize)) {
        IS_EDITING_TEST();

        if (editor_state.sel_point != nullptr) {
            ImGui::Text("Point Properties");
            ImGui::Separator();
            show_point_properties(*editor_state.sel_point);
        }
        else if (editor_state.sel_segment != nullptr) {
            ImGui::Text("Segment Properties");
            ImGui::Separator();
            show_segment_properties(*editor_state.sel_segment);
        }
        else {
            show_map_properties();
        }
    }
    ImGui::End();
}

static void show_editor_preferences_window() {
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Once);
    if (ImGui::Begin("Preferences", &editor_preferences_open)) {
        IS_EDITING_TEST();

        float line_color[3];
        float select_color[3];
        editor_state.preferences.line_color.to_array(line_color);
        editor_state.preferences.sel_point_color.to_array(select_color);

        ImGui::RadioButton("2D View", &editor_state.view_mode, EditorInterface::VIEW_2D);
        ImGui::SameLine();
        ImGui::RadioButton("3D View", &editor_state.view_mode, EditorInterface::VIEW_3D);

        ImGui::Checkbox("Draw Grid", &editor_state.preferences.draw_grid);

        ImGui::SliderInt("Point Size", &editor_state.preferences.point_size, 1, 16);
        ImGui::SliderInt("Line Width", &editor_state.preferences.line_width, 1, 16);

        ImGui::ColorEdit3("Line/Point Color", line_color);
        ImGui::ColorEdit3("Selected Color", select_color);

        editor_state.preferences.line_color.from_array(line_color);
        editor_state.preferences.sel_point_color.from_array(select_color);
    }
    ImGui::End();
}

static void show_error_popup(std::string text, bool& show) {
    ImVec2 text_size = ImGui::CalcTextSize(text.data());
    ImVec2 button_size = ImVec2(60, 25);
    ImVec2 padding = ImVec2(20, 40);
    ImVec2 window_size = ImVec2(text_size.x + button_size.x + padding.x, text_size.y + button_size.y + padding.y);

    ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);

    ImGui::OpenPopup("Error");
    ImGui::SetNextWindowFocus();
    if (ImGui::BeginPopupModal("Error")) {
        IS_EDITING_TEST();

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_size.x) / 2);
        ImGui::Text(text.data());

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - button_size.x) / 2);
        if (
            ImGui::Button("Dismiss", button_size) ||
            ImGui::Shortcut(ImGuiKey_Enter) ||
            ImGui::Shortcut(ImGuiKey_Escape)
        ) { show = false; }

        ImGui::EndPopup();
    }
}

static void show_unsaved_work_popup(bool& show) {
    std::string text = "Are you sure you want to close without saving?";

    ImVec2 text_size = ImGui::CalcTextSize(text.data());
    ImVec2 button_size = ImVec2(60, 25);
    ImVec2 padding = ImVec2(30, 40);
    ImVec2 window_size = ImVec2(text_size.x + button_size.x * 3 + padding.x, text_size.y + button_size.y * 3 + padding.y);

    ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);

    ImGui::OpenPopup("Unsaved work");
    ImGui::SetNextWindowFocus();
    if (ImGui::BeginPopupModal("Unsaved work")) {
        IS_EDITING_TEST();

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - text_size.x) / 2);
        ImGui::Text(text.data());

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - button_size.x * 3) / 2);

        if (ImGui::Button("Save", button_size)) {
            unsaved_work_popup = false;
            // TODO: SAVE !!!!!
            close_map();
        }

        ImGui::SameLine();

        if (ImGui::Button("Discard", button_size)) {
            unsaved_work_popup = false;
            map->set_changed(false);
            close_map();
        }

        ImGui::SameLine();

        if (
            ImGui::Button("Cancel", button_size) ||
            ImGui::Shortcut(ImGuiKey_Escape)
        ) {
            unsaved_work_popup = false;
        }

        ImGui::EndPopup();
    }
}

static void start_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

static void end_frame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //ImGui::EndFrame();
}

static MapVec2 screen_to_map_pos(Vec2 pos_win, MapVec2 offset, MapPositionComponent scale) {
    Vec2 pos_viewport = pos_win / Vec2(Window::get_width(), Window::get_height());
    pos_viewport *= 2.0;
    pos_viewport -= 1.0;
    pos_viewport.y *= -1;

    float aspect = (float)Window::get_width() / (float)Window::get_height();
    pos_viewport.x *= aspect;

    Vec2 mouse_pos_map_f = pos_viewport * scale;

    return MapVec2(roundf(mouse_pos_map_f.x), roundf(mouse_pos_map_f.y)) + offset;
}

static void input_2d(ImGuiIO& io) {
    if (editor_state.in_interface) { return; }

    if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
        editor_state.offset.x -= 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
        editor_state.offset.x += 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        editor_state.offset.y += 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        editor_state.offset.y -= 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Comma)) {
        editor_state.scale += 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Period)) {
        editor_state.scale -= 1;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        editor_state.input.current_tool = 0;
        editor_state.sel_point = nullptr;
        editor_state.sel_segment = nullptr;
    }

    editor_state.scale = MAX(editor_state.scale, 1);
}

static void input_3d() {
    // TODO: INPUT 3D
}

static void shortcut_inputs() {
    ImGuiInputFlags shortcut_flags = ImGuiInputFlags_RouteGlobal;

    if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_T, shortcut_flags)) {
        new_map();
    }
    else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_W, shortcut_flags)) {
        close_map();
    }
    else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, shortcut_flags)) {
        // Save
    }
    else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S, shortcut_flags)) {
        // Save as
    }
    else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, shortcut_flags)) {
        undo_command();
    }
    else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z, shortcut_flags)) {
        redo_command();
    }
}

void EditorInterface::update() {
    editor_state.in_interface = false;

    // If no map open or tools window is closed, deselect points and reset current tool.
    if (map == nullptr || editor_tools_open == false) {
        editor_state.input.current_tool = EditorInterface::TOOL_Select;
        editor_state.sel_point = nullptr;
        editor_state.sel_segment = nullptr;
    }

    start_frame();

    if (new_map_fail_popup) {
        show_error_popup("Cannot open a new map without closing this one first", new_map_fail_popup);
    }
    if (close_map_fail_popup) {
        show_error_popup("Connot close a map if no map is open ._.", close_map_fail_popup);
    }
    if (unsaved_work_popup) {
        show_unsaved_work_popup(unsaved_work_popup);
    }

    ImGui::BeginMainMenuBar();
    show_menu();
    ImGui::EndMainMenuBar();

    if (map != nullptr) {
        if (editor_tools_open) { show_tools_window(); }
        if (properties_open) { show_properties_window(); }
    }

    if (editor_preferences_open) { show_editor_preferences_window(); }

    ImGuiIO& io = ImGui::GetIO();

    editor_state.input.mouse_clicked = io.MouseClicked[0];
    editor_state.input.mouse_released = io.MouseReleased[0];
    editor_state.input.mouse_down = io.MouseDown[0];
    editor_state.input.mouse_pos_map = screen_to_map_pos(Vec2(io.MousePos.x, io.MousePos.y), editor_state.offset, editor_state.scale);

    shortcut_inputs();

    if (editor_state.view_mode == VIEW_2D) { input_2d(io); }
    else { input_3d(); }

    editor_state.map = map;

    end_frame();
}

EditorInterface::EditorState& EditorInterface::get_editor_state() { return editor_state; }
