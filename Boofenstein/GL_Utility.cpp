#include "GL_Utility.hpp"
#include "File_Utility.hpp"

#include <iostream>
#include <stdexcept>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int GL_Utility::compile_shader(unsigned int type, std::string debug_name, std::string& source) {
	unsigned int shader = glCreateShader(type);

	const char* data = source.data();

	glShaderSource(shader, 1, (const char* const*)&data, 0);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	char buffer[256];

	if (!success) {
		glGetShaderInfoLog(shader, sizeof(buffer), NULL, buffer);
		std::cout << source << '\n';
		std::cout << debug_name << ": \n" << buffer << '\n';
		throw std::logic_error("Failed to compile shader \"" + debug_name + "\"");
	}

	return shader;
}

unsigned int GL_Utility::load_shader(unsigned int type, const std::string path, char* buffer, size_t buffer_len, std::fstream& fstream) {
	//size_t src_length = File_Utility::read_file(path, buffer, buffer_len, fstream);
	std::string source = File_Utility::read_file_include(path, fstream);
	
	return compile_shader(type, path, source);
}

unsigned int GL_Utility::load_program(const std::string vert_path, const std::string frag_path) {
	std::fstream fstream;
	size_t buffer_size = 1024 * 16;
	char* buffer = new char[buffer_size];

	unsigned int vert_shader = load_shader(GL_VERTEX_SHADER, vert_path, buffer, buffer_size, fstream);
	unsigned int frag_shader = load_shader(GL_FRAGMENT_SHADER, frag_path, buffer, buffer_size, fstream);

	delete[] buffer;

	unsigned int program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return program;
}

unsigned int GL_Utility::create_texture(unsigned char* data, int width, int height, bool filtering) {
	unsigned int texture = 0;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filtering) ? GL_LINEAR : GL_NEAREST);

	glBindBuffer(GL_TEXTURE_2D, 0);

	return texture;
}

unsigned int GL_Utility::load_texture(const std::string path, bool filtering) {
	// Redundant after first load but shouldn't be an issue?
	stbi_set_flip_vertically_on_load(true);

	int w = 0, h = 0, file_channels = 0;
	unsigned char* data = stbi_load(path.data(), &w, &h, &file_channels, 4);
	unsigned int texture = create_texture(data, w, h, filtering);

	stbi_image_free(data);
	return texture;
}

unsigned int GL_Utility::create_array_texture(unsigned char** data, int num_textures, int width, int height, bool filtering) {
	unsigned int texture = 0;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, num_textures, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	// Set data
	for (int i = 0; i < num_textures; i++) {
		// Set texture at layer i
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
	}

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, (filtering) ? GL_LINEAR : GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	return texture;
}

unsigned int GL_Utility::load_array_texture(const std::string path, std::string rel_path, unsigned int* num_textures, bool filtering) {
	// Redundant after first load but shouldn't be an issue?
	stbi_set_flip_vertically_on_load(true);

	std::fstream fstream;

	std::vector<std::string> img_paths;
	File_Utility::read_file_lines(img_paths, path, fstream);

	unsigned char** data = new unsigned char*[img_paths.size()];

	int w = 0, h = 0, file_channels = 0;

	// Load textures to memory
	for (int i = 0; i < img_paths.size(); i++) {
		int curr_w = 0, curr_h = 0;

		std::string img_path_full = rel_path + img_paths[i];

		data[i] = stbi_load(img_path_full.data(), &curr_w, &curr_h, &file_channels, 4);
		
		if (w == 0 || h == 0) {
			w = curr_w;
			h = curr_h;
		}
		else if (curr_w != w || curr_h != h) {
			throw std::logic_error("All textures in array texture must have the same dimensions");
		}
	}

	unsigned int texture = create_array_texture(data, img_paths.size(), w, h, filtering);

	// Free texture data from memory
	for (int i = 0; i < img_paths.size(); i++) {
		stbi_image_free(data[i]);
	}

	delete[] data;
	
	*num_textures = img_paths.size();
	return texture;
}

unsigned int GL_Utility::create_empty_texture(int width, int height, bool filtering, unsigned int internal_format, unsigned int format, unsigned int type) {
	unsigned int texture = 0;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (filtering) ? GL_LINEAR : GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

unsigned int GL_Utility::create_framebuffer(unsigned int color_texture, unsigned int depth_texture) {
	unsigned int framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::logic_error("Framebuffer is not complete!! Framebuffer " + std::to_string(framebuffer));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return framebuffer;
}

unsigned int GL_Utility::create_framebuffer(unsigned int color_texture) {
	unsigned int framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::logic_error("Framebuffer is not complete!! Framebuffer " + std::to_string(framebuffer));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return framebuffer;
}
