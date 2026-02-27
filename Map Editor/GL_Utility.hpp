#pragma once

#include <string>
#include <fstream>

namespace GL_Utility {
	unsigned int compile_shader(unsigned int type, std::string debug_name, std::string& source);
	unsigned int load_shader(unsigned int gl_type, const std::string path, char* buffer, size_t buffer_len, std::fstream& fstream);
	unsigned int load_program(const std::string vert_path, const std::string frag_path);
	
	unsigned int create_texture(unsigned char* data, int width, int height, bool filtering);
	unsigned int load_texture(const std::string path, bool filtering);

	unsigned int create_array_texture(unsigned char** data, int num_textures, int width, int height, bool filtering);
	unsigned int load_array_texture(const std::string* paths, unsigned int num_textures, bool filtering);
	//unsigned int load_array_texture(const std::string path, std::string rel_path, unsigned int* num_textures, bool filtering);

	unsigned int create_empty_texture(int width, int height, bool filtering, unsigned int internal_format, unsigned int format, unsigned int type);
	unsigned int create_framebuffer(unsigned int color_texture, unsigned int depth_texture);
	unsigned int create_framebuffer(unsigned int color_texture);
};