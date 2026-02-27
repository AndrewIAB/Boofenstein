#include "RendererBase.hpp"

#include <iostream>

#include <glad/glad.h>

#include "Window.hpp"
#include "GL_Utility.hpp"

RendererBase::RendererBase() {
	// Vertex Attribute Objects
	glGenVertexArrays(1, &vertex_attrib_object);

	// Buffers
	glGenBuffers(1, &vertex_buffer.id);
	
	// Textures
	// TODO
}

RendererBase::~RendererBase() {
	// Buffers
	glDeleteBuffers(1, &vertex_buffer.id);

	// Textures
	// TODO

	free_shaders();
}

void RendererBase::add_vertex_attribute(const RVertexAttribute& attribute) {
	vertex_attributes.push_back(attribute);
}

static GLenum get_gl_type(RType render_type) {
	switch (render_type) {
	case RTYPE_FLOAT:
		return GL_FLOAT;
	case RTYPE_INT:
		return GL_INT;
	case RTYPE_UNSIGNED_INT:
		return GL_UNSIGNED_INT;
	}
}

static size_t get_type_size(RType render_type) {
	switch (render_type) {
	case RTYPE_FLOAT:
		return sizeof(float);
	case RTYPE_INT:
		return sizeof(int);
	case RTYPE_UNSIGNED_INT:
		return sizeof(unsigned int);
	}
}

typedef std::list<RVertexAttribute> VertAttribList;
typedef VertAttribList::iterator VertAttribIter;

static size_t get_attributes_stride(VertAttribList vertex_attributes) {
	size_t total_stride = 0;

	for (VertAttribIter it = vertex_attributes.begin(); it != vertex_attributes.end(); ++it) {
		const RVertexAttribute& attrib = *it;

		total_stride += attrib.size * get_type_size(attrib.type);
	}

	return total_stride;
}

void RendererBase::apply_vertex_attributes() {
	size_t total_stride = get_attributes_stride(vertex_attributes);
	size_t offset = 0;

	int index = 0;

	glBindVertexArray(vertex_attrib_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);

	VertAttribIter it = vertex_attributes.begin();
	
	while (it != vertex_attributes.end()) {
		const RVertexAttribute& attrib = *it;

		glVertexAttribPointer(index, attrib.size, get_gl_type(attrib.type), attrib.normalized, total_stride, (void*)offset);
		glEnableVertexAttribArray(index);

		offset += attrib.size * get_type_size(attrib.type);
		index++;

		it = vertex_attributes.erase(it);
	}
}

void RendererBase::alloc_buffers(unsigned long long size) {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);


	vertex_buffer.size = size;
	vertex_buffer.used = 0;

	num_vertices_pushed = 0;
}

RID RendererBase::load_texture(const std::string path, bool filtering) {
	textures.push_back(GL_Utility::load_texture(path, filtering));
	return textures.back();
}

void RendererBase::free_textures() {
	for (int i = 0; i < textures.size(); i++) {
		glDeleteTextures(1, &textures[i]);
	}
	textures.clear();
}

void RendererBase::bind_texture_2D(RID texture, int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

RID RendererBase::load_shader(const std::string& vert_path, const std::string& frag_path) {
	shaders.push_back(GL_Utility::load_program(vert_path, frag_path));
	return shaders.back();
}

void RendererBase::free_shaders() {
	for (int i = 0; i < shaders.size(); i++) {
		glDeleteShader(shaders[i]);
	}
	shaders.clear();
}

RID RendererBase::get_uniform(RID shader, const std::string& name) {
	unsigned int id = glGetUniformLocation(shader, name.data());

	if (id == -1) {
		std::cout << "WARNING: Uniform \"" << name << "\" is not found in shader " << shader << ".\n";
		//throw std::logic_error("Uniform \"" + name + "\" is not found in shader " + std::to_string(shader));
	}

	return id;
}

void RendererBase::set_uniform(RID shader, RID location, int a) {
	glUseProgram(shader);
	glUniform1i(location, a);
}

void RendererBase::set_uniform(RID shader, RID location, float a) {
	glUseProgram(shader);
	glUniform1f(location, a);
}

void RendererBase::set_uniform(RID shader, RID location, Vec2 v) {
	glUseProgram(shader);
	glUniform2f(location, v.x, v.y);
}

void RendererBase::set_uniform(RID shader, RID location, Vec3 v) {
	glUseProgram(shader);
	glUniform3f(location, v.x, v.y, v.z);
}

void RendererBase::set_uniform(RID shader, RID location, Vec4 v) {
	glUseProgram(shader);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void RendererBase::set_uniform(RID shader, RID location, Mat3 mat) {
	glUseProgram(shader);

	float data[] = {
		mat.r1.x, mat.r2.x, mat.r3.x,
		mat.r1.y, mat.r2.y, mat.r3.y,
		mat.r1.z, mat.r2.z, mat.r3.z
	};

	glUniformMatrix3fv(location, 1, GL_FALSE, data);
}

void RendererBase::set_uniform(RID shader, RID location, Mat4 mat) {
	glUseProgram(shader);

	float data[] = {
		mat.r1.x, mat.r2.x, mat.r3.x, mat.r4.x,
		mat.r1.y, mat.r2.y, mat.r3.y, mat.r4.y,
		mat.r1.z, mat.r2.z, mat.r3.z, mat.r4.z,
		mat.r1.w, mat.r2.w, mat.r3.w, mat.r4.w
	};

	glUniformMatrix4fv(location, 1, GL_FALSE, data);
}

RModel RendererBase::push_model(const float* vertex_data, unsigned long long size, unsigned int num_vertices) {
	RModel model = {num_vertices_pushed, num_vertices};

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);
	glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer.used, size, vertex_data);

	vertex_buffer.used += size;

	num_vertices_pushed += num_vertices;

	return model;
}

void RendererBase::enable_depth_test() {
	glEnable(GL_DEPTH_TEST);
}

void RendererBase::disable_depth_test() {
	glDisable(GL_DEPTH_TEST);
}

void RendererBase::cull_front_faces() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void RendererBase::cull_back_faces() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void RendererBase::disable_cull_faces() {
	glDisable(GL_CULL_FACE);
}

void RendererBase::draw_triangles(RID shader, const RModel& model, bool wireframe, float wireframe_width) {
	glBindVertexArray(vertex_attrib_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);
	glUseProgram(shader);
	
	if (wireframe) {
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(wireframe_width);
	}
	else {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	glDrawArrays(GL_TRIANGLES, model.start_index, model.num_vertices);
}

void RendererBase::draw_lines(RID shader, const RModel& model, float width) {
	glBindVertexArray(vertex_attrib_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);
	glUseProgram(shader);

	glLineWidth(width);

	glDrawArrays(GL_TRIANGLES, model.start_index, model.num_vertices);
}

void RendererBase::draw_points(RID shader, const RModel& model, float size) {
	glBindVertexArray(vertex_attrib_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.id);
	glUseProgram(shader);

	glPointSize(size);

	glDrawArrays(GL_POINTS, model.start_index, model.num_vertices);
}