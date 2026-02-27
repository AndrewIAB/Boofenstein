#include "Window.hpp"

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static GLFWwindow* window = nullptr;
static bool init_glad = false;
static int width, height;
static void (*key_callback)(int, bool, bool) = nullptr;

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key_callback == nullptr) {
		return;
	}

	//const char* str_char = glfwGetKeyName(key, scancode);
	//
	//if (str_char == nullptr) {
	//	return;
	//}

	key_callback(key, action == GLFW_PRESS, action == GLFW_RELEASE);
}

void Window::init(int w, int h, std::string name, bool is_fullscreen) {
	if (window != nullptr) {
		return;
	}

	if (!glfwInit()) {
		throw std::runtime_error("Window::init -> Failed to initialize GLFW.");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWmonitor* monitor = is_fullscreen ? glfwGetPrimaryMonitor() : NULL;

	width = w;
	height = h;

	window = glfwCreateWindow(width, height, name.data(), monitor, NULL);

	if (window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Window::init -> Failed to create GLFW window.");
	}
	
	glfwMakeContextCurrent(window);
	
	glfwSetKeyCallback(window, glfw_key_callback);

	if (!init_glad) {
		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			init_glad = true;
		}
		else {
			throw std::runtime_error("Window::init -> Failed to initizalize GLAD.");
		}
	}

	glViewport(0, 0, width, height);
}

void Window::quit() {
	glfwTerminate();
	window = nullptr;
}

bool Window::poll() { glfwPollEvents();  return !glfwWindowShouldClose(window) && window != nullptr; }
void Window::swap() { glfwSwapBuffers(window); }

void Window::set_key_callback(void (*callback)(int, bool, bool)) {
	key_callback = callback;
}

void* Window::get_window_ptr() { return window; }
int Window::get_width() { return width; }
int Window::get_height() { return height; }