#pragma once

#include "Vec_n.hpp"

#include <string>
#include <vector>
#include <list>

typedef unsigned int RID;

enum RType {
	RTYPE_FLOAT,
	RTYPE_INT,
	RTYPE_UNSIGNED_INT
};

struct RBuffer {
	RID id;
	unsigned long long size;
	unsigned long long used;
};

struct RVertexAttribute {
	unsigned int size;
	RType type;
	bool normalized;
};

struct RModel {
	unsigned int start_index;
	unsigned int num_vertices;
};

class RendererBase {
	RBuffer vertex_buffer = {0, 0, 0};
	RID vertex_attrib_object = 0;

	unsigned int num_vertices_pushed = 0;

	std::vector<RID> shaders;
	std::vector<RID> textures;

	std::list<RVertexAttribute> vertex_attributes;
public:
	RendererBase();
	~RendererBase();

	// Specify vertex attribute data
	void add_vertex_attribute(const RVertexAttribute& attribute);
	void apply_vertex_attributes();

	// Allocate vertex buffer
	void alloc_buffers(unsigned long long size);

	// Initialize texture array
	RID load_texture(const std::string path, bool filtering);
	void free_textures();
	void bind_texture_2D(RID texture, int unit);

	// Compile shader program and return ID
	RID load_shader(const std::string& vert_path, const std::string& frag_path);
	void free_shaders();

	// Get uniform from shader and return location
	RID get_uniform(RID shader, const std::string& name);
	
	void set_uniform(RID shader, RID location, int a);
	void set_uniform(RID shader, RID location, float a);
	void set_uniform(RID shader, RID location, Vec2 v);
	void set_uniform(RID shader, RID location, Vec3 v);
	void set_uniform(RID shader, RID location, Vec4 v);
	void set_uniform(RID shader, RID location, Mat3 mat);
	void set_uniform(RID shader, RID location, Mat4 mat);

	// Write data to current vertex buffer
	RModel push_model(const float* vertex_data, unsigned long long size, unsigned int num_vertices);

	void enable_depth_test();
	void disable_depth_test();

	void cull_front_faces();
	void cull_back_faces();
	void disable_cull_faces();

	void draw_triangles(RID shader, const RModel& model, bool wireframe = false, float wireframe_width = 0);
	void draw_lines(RID shader, const RModel& model, float width);
	void draw_points(RID shader, const RModel& model, float size);
};