// Collection of tests using modern OpenGL

#include <iostream>
#include <stdexcept>
#include <string>
#include <list>
#include <cmath>
#include <chrono>

#include <GLFW/glfw3.h>

#include "File_Utility.hpp"
#include "Window.hpp"
#include "SegmentPathRenderer.hpp"

#define PI 3.14159
#define RADIAN (PI / 180)

static struct {
	bool forward	= false;
	bool backward	= false;
	bool left		= false;
	bool right		= false;
	bool alt_move	= false;
} input_data;

double get_delta(std::chrono::steady_clock::time_point now, std::chrono::steady_clock::time_point then) {
	std::chrono::steady_clock::duration delta = now - then;
	std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::microseconds>(delta);
	return (float)ms.count() * 0.000001;
}

void key_callback(int key, bool key_press, bool key_release) {
	bool pressed = key_press && !key_release || !(key_press || key_release);

	if (key == GLFW_KEY_ESCAPE) {
		Window::quit();
		return;
	}

	switch (key) {
	case GLFW_KEY_LEFT_ALT:
		input_data.alt_move = pressed;
		break;

	case GLFW_KEY_UP:
		input_data.forward = pressed;
		break;
	case GLFW_KEY_DOWN:
		input_data.backward = pressed;
		break;

	case GLFW_KEY_LEFT:
		input_data.left = pressed;
		break;
	case GLFW_KEY_RIGHT:
		input_data.right = pressed;
		break;
	}
}

int main() {
	std::chrono::steady_clock clock;
	SegmentPathRenderer* segment_renderer = nullptr;

	// Yuck
	try {
		Window::init(700, 700, "Render", false);

		int side = 300;
		int groups = 4;

		segment_renderer = new SegmentPathRenderer(side, side, groups, groups, "assets");
	}
	catch (const std::runtime_error& e) {
		std::cout << "RUNTIME ERROR:\n" << e.what() << '\n';
		return -1;
	}
	catch (const std::logic_error& e) {
		std::cout << "LOGIC ERROR:\n" << e.what() << '\n';
		return -1;
	}
	catch (const std::exception& e) {
		// Hehehehehehe
		std::cout << "GENERAL ERROR:\n" << e.what() << '\n';
		return -1;
	}

	Window::set_key_callback(key_callback);

	std::vector<SegmentMaterial> materials;

	int sample_material, cat_material, dog_material, light_material, wall_material;

	// 0 - Sample
	sample_material = 0;
	materials.push_back(SegmentMaterial(
		0, 7, -1, -1,
		Vec3(), 0.4, Vec3(),
		Vec2(0.75, 0.75), Vec2(0.0, 0.0)
	));
	// 1 - Cat
	cat_material = 1;
	materials.push_back(SegmentMaterial(1, 0.0, Vec3(0.0, 0.0, 0.0)));
	// 2 - Dog
	dog_material = 2;
	materials.push_back(SegmentMaterial(2, 0.0, Vec3(1.5, 0.5, 0.5)));
	// 3 - Light Wall
	light_material = 3;
	materials.push_back(SegmentMaterial(
		3, -1, -1, 3,
		Vec3(), 0.0, Vec3(0.5, 0.5, 1.5)
	));
	// 4 - Brick Wall
	wall_material = 4;
	materials.push_back(SegmentMaterial(
		4, 6, 5, -1,
		Vec3(), 0.1, Vec3(),
		Vec2(0.5, 0.5), Vec2(0.0, 0.0)
	));

	std::list<SegmentNode> segments;
	segments.push_back(SegmentNode(Vec2(+3.0, +3.0), Vec2(-3.0, +3.0), cat_material));
	segments.push_back(SegmentNode(Vec2(-3.0, -3.0), Vec2(+3.0, -3.0), cat_material));
	segments.push_back(SegmentNode(Vec2(+3.0, -3.0), Vec2(+3.0, +3.0), cat_material));
	segments.push_back(SegmentNode(Vec2(-3.0, +3.0), Vec2(-3.0, -3.0), cat_material));

	segments.push_back(SegmentNode(Vec2(-10.0, +10.0), Vec2(+10.0, +10.0), light_material));
	segments.push_back(SegmentNode(Vec2(+10.0, -10.0), Vec2(-10.0, -10.0), dog_material));
	segments.push_back(SegmentNode(Vec2(+10.0, +10.0), Vec2(+10.0, -10.0), wall_material));
	segments.push_back(SegmentNode(Vec2(-10.0, -10.0), Vec2(-10.0, +10.0), wall_material));

	segment_renderer->set_materials(materials);
	segment_renderer->set_segments(segments, materials, 2.0, -1.0, sample_material, sample_material);
	segment_renderer->set_iterations(1);

	segment_renderer->set_perspective(0.001, 50.0, RADIAN * 60);

	float move_speed = 6.0;
	float look_speed = 3.0;
	float rotation = 0.0;
	Vec3 position = Vec3(0, 0, -2);

	std::chrono::steady_clock::time_point post_frame;
	std::chrono::steady_clock::time_point pre_frame;

	while (Window::poll()) {
		float delta = get_delta(post_frame, pre_frame);

		std::cout << 1.0 / delta << '\n';

		pre_frame = clock.now();

		Vec3 forward_vec = Vec3(sin(rotation), 0, cos(rotation));
		Vec3 right_vec = Vec3(cos(rotation), 0, -sin(rotation));

		float delta_move_speed = move_speed * delta;
		float delta_look_speed = look_speed * delta;

		if (input_data.forward) {
			position += forward_vec * delta_move_speed;
		}
		if (input_data.backward) {
			position -= forward_vec * delta_move_speed;
		}
		if (input_data.right) {
			if (input_data.alt_move) {
				position += right_vec * delta_move_speed;
			}
			else {
				rotation += delta_look_speed;
			}
		}
		if (input_data.left) {
			if (input_data.alt_move) {
				position -= right_vec * delta_move_speed;
			}
			else {
				rotation -= delta_look_speed;
			}
		}

		if (rotation > 2 * PI) {
			rotation = 2 * PI - rotation;
		}

		segment_renderer->set_view_pos(position);
		segment_renderer->set_view_rotation(rotation);

		segment_renderer->draw();
		Window::swap();

		post_frame = clock.now();
	}

	delete segment_renderer;
	Window::quit();
	return 0;
}