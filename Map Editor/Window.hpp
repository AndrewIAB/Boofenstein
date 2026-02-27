#pragma once

#include <string>

namespace Window {
	void init(int w, int h, std::string name, bool is_resizeable);
	void quit();
	bool poll();
	void swap();

	int get_key_code(std::string name);
	void set_key_callback(void (*callback)(int key, bool pressed, bool released));

	void* get_window_ptr();

	int get_width();
	int get_height();

	void clear_color(float r, float g, float b);
	void clear();
};