#include "SegmentPathRenderer.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include "Window.hpp"
#include "File_Utility.hpp"
#include "GL_Utility.hpp"

//void SegmentMaterial::load_texture(std::string path) {
//	if (gl_texture != 0) {
//		glDeleteTextures(1, &gl_texture);
//	}
//
//	gl_texture = GL_Utility::load_texture(path, true);
//}

SegmentPathRenderer::SegmentPathRenderer(int render_w, int render_h, int num_groups_x, int num_groups_y, std::string asset_path) {
	//glClearColor(1.0, 0.0, 0.0, 1.0);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);
	
	this->render_w = render_w;
	this->render_h = render_h;

	// Setup segment buffer, don't supply data yet
	glGenBuffers(1, &segment_ubo);
	glGenBuffers(1, &material_ubo);

	// Textures & Framebuffers

	std::string asset_folder = asset_path;

	if (asset_folder[asset_folder.length() - 1] != '\\' && asset_folder[asset_folder.length() - 1] != '/') {
		asset_folder += '\\';
	}

	texture_array = GL_Utility::load_array_texture(asset_folder + "texture_array.txt", asset_folder, &num_textures, true);

	depth_buffer_texture	= GL_Utility::create_empty_texture(render_w, render_h, false,	GL_DEPTH_COMPONENT32F,	GL_DEPTH_COMPONENT,	GL_FLOAT);
	
	position_texture	= GL_Utility::create_empty_texture(render_w, render_h, true,	GL_RGBA32F, GL_RGBA, GL_FLOAT);
	surface_texture		= GL_Utility::create_empty_texture(render_w, render_h, false,	GL_RGBA32F, GL_RGBA, GL_FLOAT);
	normal_texture		= GL_Utility::create_empty_texture(render_w, render_h, false,	GL_RGBA32F, GL_RGBA, GL_FLOAT);
	ray_texture			= GL_Utility::create_empty_texture(render_w, render_h, true,	GL_RGBA32F,	GL_RGBA, GL_FLOAT);
	
	position_framebuffer	= GL_Utility::create_framebuffer(position_texture, depth_buffer_texture);
	surface_framebuffer		= GL_Utility::create_framebuffer(surface_texture, depth_buffer_texture);
	normal_framebuffer		= GL_Utility::create_framebuffer(normal_texture, depth_buffer_texture);
	ray_framebuffer		= GL_Utility::create_framebuffer(ray_texture);

	// Programs

	position_raster_program	= GL_Utility::load_program(asset_folder + "raster.vert", asset_folder + "position_raster.frag");
	surface_raster_program	= GL_Utility::load_program(asset_folder + "raster.vert", asset_folder + "surface_raster.frag");
	normal_raster_program	= GL_Utility::load_program(asset_folder + "raster.vert", asset_folder + "normal_raster.frag");
	glGenVertexArrays(1, &raster_vao);
	glGenBuffers(1, &raster_vbo);
	raster_u_offset = glGetUniformLocation(position_raster_program, "u_offset");
	raster_u_rotation = glGetUniformLocation(position_raster_program, "u_rotation");
	raster_u_perspective = glGetUniformLocation(position_raster_program, "u_perspective");
	glUseProgram(position_raster_program);
	glUniform1i(glGetUniformLocation(normal_raster_program, "u_texture_array"), 0);
	glUseProgram(surface_raster_program);
	glUniform1i(glGetUniformLocation(normal_raster_program, "u_texture_array"), 0);
	glUseProgram(normal_raster_program);
	glUniform1i(glGetUniformLocation(normal_raster_program, "u_texture_array"), 0);
	// UBO Bindings
	//glUniformBlockBinding(position_raster_program,	glGetUniformBlockIndex(ray_program, "ub_Materials"), 0);
	//glUniformBlockBinding(surface_raster_program,	glGetUniformBlockIndex(ray_program, "ub_Materials"), 0);
	glUniformBlockBinding(normal_raster_program,	glGetUniformBlockIndex(ray_program, "ub_Materials"), 0);

	ray_program = GL_Utility::load_program(asset_folder + "ray.vert", asset_folder + "ray.frag");
	glGenVertexArrays(1, &ray_vao);
	glGenBuffers(1, &ray_vbo);
	ray_u_offset		= glGetUniformLocation(ray_program, "u_offset");
	ray_u_rotation		= glGetUniformLocation(ray_program, "u_rotation");
	ray_u_iterations	= glGetUniformLocation(ray_program, "u_iterations");
	ray_u_far			= glGetUniformLocation(ray_program, "u_far");
	ray_u_fov			= glGetUniformLocation(ray_program, "u_fov");
	ray_u_max_y			= glGetUniformLocation(ray_program, "u_max_y");
	ray_u_min_y			= glGetUniformLocation(ray_program, "u_min_y");
	glUseProgram(ray_program);
	glUniform1i(glGetUniformLocation(ray_program, "u_texture_array"),		0);
	glUniform1i(glGetUniformLocation(ray_program, "u_texture_position"),	1);
	glUniform1i(glGetUniformLocation(ray_program, "u_texture_surface"),		2);
	glUniform1i(glGetUniformLocation(ray_program, "u_texture_normal"),		3);
	// Set segments UBO to binding 0
	glUniformBlockBinding(ray_program, glGetUniformBlockIndex(ray_program, "ub_Segments"),	0);
	// Set materials UBO to binding 1
	glUniformBlockBinding(ray_program, glGetUniformBlockIndex(ray_program, "ub_Materials"),	1);

	post_program = GL_Utility::load_program(asset_folder + "post.vert", asset_folder + "post.frag");
	glUseProgram(post_program);
	glUniform1i(glGetUniformLocation(post_program, "u_texture_ray"), 0);
	glUniform1i(glGetUniformLocation(post_program, "u_texture_position"), 1);
	//glUniform1i(glGetUniformLocation(post_program, "u_texture_depth"), 1);
	glUniform2f(glGetUniformLocation(post_program, "u_resolution"), render_w, render_h);

	glBindVertexArray(raster_vao);
	glBindBuffer(GL_ARRAY_BUFFER, raster_vbo);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(0));
	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(3 * sizeof(float)));
	// Tangent
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(6 * sizeof(float)));
	// Bitangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(9 * sizeof(float)));
	// UV
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(12 * sizeof(float)));
	// Material ID
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(14 * sizeof(float)));
	// Wall ID
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, DATA_PER_VERT, (void*)(15 * sizeof(float)));

	glBindVertexArray(ray_vao);
	glBindBuffer(GL_ARRAY_BUFFER, ray_vbo);

	// 2D Screen Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));

	ray_num_triangles = (num_groups_x * num_groups_y) * 2;
	float group_w = 1.0 / (float)num_groups_x;
	float group_h = 1.0 / (float)num_groups_y;

	glBindBuffer(GL_ARRAY_BUFFER, ray_vbo);

	glBufferData(GL_ARRAY_BUFFER, ray_num_triangles * sizeof(float) * 6, NULL, GL_DYNAMIC_DRAW);

	// Yuck
	// Creating vertices for groups
	// Something tells me a compute shader would've been easier :p
	for (int i = 0; i < num_groups_x * num_groups_y; i++) {
		int ix = i % num_groups_x;
		int iy = i / num_groups_x;

		float x1 = (group_w * (float)ix) * 2.0 - 1.0;
		float y1 = (group_h * (float)iy) * 2.0 - 1.0;
		float x2 = x1 + group_w * 2.0;
		float y2 = y1 + group_h * 2.0;

		float data[] = {
			x1, y1,
			x2, y2,
			x1, y2,

			x2, y2,
			x1, y1,
			x2, y1
		};

		glBufferSubData(GL_ARRAY_BUFFER, i * sizeof(float) * 12, sizeof(float) * 12, data);
	}
}

SegmentPathRenderer::~SegmentPathRenderer() {
	// TODO!!!!!!! RENDERER
}

static Vec2 get_normal(SegmentNode& wall) {
	Vec2 delta = wall.b - wall.a;
	delta.normalize();
	return Vec2(delta.y, -delta.x);
}

void SegmentPathRenderer::set_materials(std::vector<SegmentMaterial>& material_list) {
	glBindBuffer(GL_UNIFORM_BUFFER, material_ubo);
	
	glBufferData(GL_UNIFORM_BUFFER, material_list.size() * (SegmentMaterial::PADDED_SIZE), NULL, GL_DYNAMIC_DRAW);

	GLintptr offset = 0;

	for (std::vector<SegmentMaterial>::iterator it = material_list.begin(); it != material_list.end(); ++it) {
		float data[] = {
			it->albedo_texture_layer, it->normal_texture_layer, it->specular_texture_layer, it->emissive_texture_layer,
			it->base_albedo.x, it->base_albedo.y, it->base_albedo.z,
			it->base_specular,
			it->base_emissive.x, it->base_emissive.y, it->base_emissive.z,
			it->uv_scale.x, it->uv_scale.y,
			it->uv_offset.x, it->uv_offset.y
		};
		
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(data), data);
		
		offset += SegmentMaterial::PADDED_SIZE;
	}
}

void SegmentPathRenderer::set_segments(std::list<SegmentNode>& segment_list, std::vector<SegmentMaterial>& materials, float max_y, float min_y, int roof_material_id, int floor_material_id) {
	raster_num_triangles = (segment_list.size() + 2) * 2;
	glBindBuffer(GL_ARRAY_BUFFER, raster_vbo);
	glBindBuffer(GL_UNIFORM_BUFFER, segment_ubo);

	// Allocate for raster models
	glBufferData(GL_ARRAY_BUFFER, raster_num_triangles * DATA_PER_VERT * 3, NULL, GL_DYNAMIC_DRAW);
	// Allocate for raytrace geometry
	glBufferData(GL_UNIFORM_BUFFER, segment_list.size() * SegmentNode::PADDED_SIZE, NULL, GL_DYNAMIC_DRAW);

	GLintptr raster_offset = 0;
	GLintptr ray_offset = 0;

	float min_x = segment_list.front().a.x, max_x = segment_list.front().a.x;
	float min_z = segment_list.front().a.y, max_z = segment_list.front().a.y;

	float y_diff = max_y - min_y;
	int wall_id = 0;

	for (std::list<SegmentNode>::iterator it = segment_list.begin(); it != segment_list.end(); ++it) {
		// RASTER //

		SegmentMaterial& mat = materials[it->material_id];

		min_x = std::min(std::min(it->a.x, it->b.x), min_x);
		max_x = std::max(std::max(it->a.x, it->b.x), max_x);

		min_z = std::min(std::min(it->a.y, it->b.y), min_z);
		max_z = std::max(std::max(it->a.y, it->b.y), max_z);

		Vec2 normal = get_normal(*it);
		float length = (it->b - it->a).length();

		Vec2 delta = it->b - it->a;
		Vec3 tan = Vec3(0.0, 1.0, 0.0);
		Vec3 bitan = Vec3(-normal.y, 0.0, normal.x);

		float raster_data[] = {
			it->a.x, max_y, it->a.y,	normal.x, 0.0, normal.y,	tan.x, tan.y, tan.z,	bitan.x, bitan.y, bitan.z,	mat.uv_offset.x+mat.uv_scale.x*0.0,		mat.uv_offset.y+mat.uv_scale.y*y_diff,	it->material_id, (float)wall_id,
			it->a.x, min_y, it->a.y,	normal.x, 0.0, normal.y,	tan.x, tan.y, tan.z,	bitan.x, bitan.y, bitan.z,	mat.uv_offset.x+mat.uv_scale.x*0.0,		mat.uv_offset.y+mat.uv_scale.y*0.0,		it->material_id, (float)wall_id,
			it->b.x, min_y, it->b.y,	normal.x, 0.0, normal.y,	tan.x, tan.y, tan.z,	bitan.x, bitan.y, bitan.z,	mat.uv_offset.x+mat.uv_scale.x*length,	mat.uv_offset.y+mat.uv_scale.y*0.0,		it->material_id, (float)wall_id,
			it->b.x, max_y, it->b.y,	normal.x, 0.0, normal.y,	tan.x, tan.y, tan.z,	bitan.x, bitan.y, bitan.z,	mat.uv_offset.x+mat.uv_scale.x*length,	mat.uv_offset.y+mat.uv_scale.y*y_diff,	it->material_id, (float)wall_id,
			it->a.x, max_y, it->a.y,	normal.x, 0.0, normal.y,	tan.x, tan.y, tan.z,	bitan.x, bitan.y, bitan.z,	mat.uv_offset.x+mat.uv_scale.x*0.0,		mat.uv_offset.y+mat.uv_scale.y*y_diff,	it->material_id, (float)wall_id,
			it->b.x, min_y, it->b.y,	normal.x, 0.0, normal.y,	tan.x, tan.y, tan.z,	bitan.x, bitan.y, bitan.z,	mat.uv_offset.x+mat.uv_scale.x*length,	mat.uv_offset.y+mat.uv_scale.y*0.0,		it->material_id, (float)wall_id
		};

		glBufferSubData(GL_ARRAY_BUFFER, raster_offset, sizeof(raster_data), raster_data);
		raster_offset += sizeof(raster_data);

		wall_id++;

		// RAY //

		float ray_data[] = {
			// Points
			it->a.x, it->a.y,
			it->b.x, it->b.y,
			it->material_id
		};
		
		glBufferSubData(GL_UNIFORM_BUFFER, ray_offset, sizeof(ray_data), ray_data);
		ray_offset += SegmentNode::PADDED_SIZE;
	}

	SegmentMaterial& f_mat = materials[floor_material_id];
	SegmentMaterial& r_mat = materials[roof_material_id];

	float raster_data[] = {
		min_x, min_y, min_z,	0, +1, 0,	1, 0, 0,	0, 0, 1,	f_mat.uv_offset.x+f_mat.uv_scale.x*min_x, f_mat.uv_offset.y+f_mat.uv_scale.y*min_z,	floor_material_id,	-2,
		max_x, min_y, max_z,	0, +1, 0,	1, 0, 0,	0, 0, 1,	f_mat.uv_offset.x+f_mat.uv_scale.x*max_x, f_mat.uv_offset.y+f_mat.uv_scale.y*max_z,	floor_material_id,	-2,
		min_x, min_y, max_z,	0, +1, 0,	1, 0, 0,	0, 0, 1,	f_mat.uv_offset.x+f_mat.uv_scale.x*min_x, f_mat.uv_offset.y+f_mat.uv_scale.y*max_z,	floor_material_id,	-2,
		
		min_x, min_y, min_z,	0, +1, 0,	1, 0, 0,	0, 0, 1,	f_mat.uv_offset.x+f_mat.uv_scale.x*min_x, f_mat.uv_offset.y+f_mat.uv_scale.y*min_z,	floor_material_id,	-2,
		max_x, min_y, min_z,	0, +1, 0,	1, 0, 0,	0, 0, 1,	f_mat.uv_offset.x+f_mat.uv_scale.x*max_x, f_mat.uv_offset.y+f_mat.uv_scale.y*min_z,	floor_material_id,	-2,
		max_x, min_y, max_x,	0, +1, 0,	1, 0, 0,	0, 0, 1,	f_mat.uv_offset.x+f_mat.uv_scale.x*max_x, f_mat.uv_offset.y+f_mat.uv_scale.y*max_x,	floor_material_id,	-2,

		min_x, max_y, min_z,	0, -1, 0,	1, 0, 0,	0, 0, 1,	r_mat.uv_offset.x+r_mat.uv_scale.x*min_x, r_mat.uv_offset.y+r_mat.uv_scale.y*min_z,	roof_material_id,	-3,
		min_x, max_y, max_z,	0, -1, 0,	1, 0, 0,	0, 0, 1,	r_mat.uv_offset.x+r_mat.uv_scale.x*min_x, r_mat.uv_offset.y+r_mat.uv_scale.y*max_z,	roof_material_id,	-3,
		max_x, max_y, max_z,	0, -1, 0,	1, 0, 0,	0, 0, 1,	r_mat.uv_offset.x+r_mat.uv_scale.x*max_x, r_mat.uv_offset.y+r_mat.uv_scale.y*max_z,	roof_material_id,	-3,
		
		min_x, max_y, min_z,	0, -1, 0,	1, 0, 0,	0, 0, 1,	r_mat.uv_offset.x+r_mat.uv_scale.x*min_x, r_mat.uv_offset.y+r_mat.uv_scale.y*min_z,	roof_material_id,	-3,
		max_x, max_y, max_x,	0, -1, 0,	1, 0, 0,	0, 0, 1,	r_mat.uv_offset.x+r_mat.uv_scale.x*max_x, r_mat.uv_offset.y+r_mat.uv_scale.y*max_x,	roof_material_id,	-3,
		max_x, max_y, min_z,	0, -1, 0,	1, 0, 0,	0, 0, 1,	r_mat.uv_offset.x+r_mat.uv_scale.x*max_x, r_mat.uv_offset.y+r_mat.uv_scale.y*min_z,	roof_material_id,	-3
	};

	glBufferSubData(GL_ARRAY_BUFFER, raster_offset, sizeof(raster_data), raster_data);
	raster_offset += sizeof(raster_data);

	glUseProgram(ray_program);
	glUniform1f(ray_u_max_y, max_y);
	glUniform1f(ray_u_min_y, min_y);
	glUniform1i(glGetUniformLocation(ray_program, "u_num_segments"), segment_list.size());
	glUniform1i(glGetUniformLocation(ray_program, "u_roof_material_id"), roof_material_id);
	glUniform1i(glGetUniformLocation(ray_program, "u_floor_material_id"), floor_material_id);
}

void SegmentPathRenderer::set_view_pos(Vec3 v) {
	glUseProgram(position_raster_program);
	glUniform3f(raster_u_offset, v.x, v.y, v.z);
	
	glUseProgram(surface_raster_program);
	glUniform3f(raster_u_offset, v.x, v.y, v.z);
	
	glUseProgram(normal_raster_program);
	glUniform3f(raster_u_offset, v.x, v.y, v.z);
	
	glUseProgram(ray_program);
	glUniform3f(ray_u_offset, v.x, v.y, v.z);
}
void SegmentPathRenderer::set_view_rotation(float a) {
	glUseProgram(position_raster_program);
	glUniform1f(raster_u_rotation, a);
	
	glUseProgram(surface_raster_program);
	glUniform1f(raster_u_rotation, a);
	
	glUseProgram(normal_raster_program);
	glUniform1f(raster_u_rotation, a);
	
	glUseProgram(ray_program);
	glUniform1f(ray_u_rotation, a);
}

void SegmentPathRenderer::set_perspective(float near, float far, float fov) {
	float inv_tan = 1.0 / tan(fov * 0.5);
	float inv_diff = 1.0 / (far - near);
	float mat[] = {
		inv_tan, 0, 0, 0,
		0, inv_tan, 0, 0,
		0, 0, (far + near) * inv_diff, -2 * far * near * inv_diff,
		0, 0, 1, 0
	};

	glUseProgram(position_raster_program);
	glUniformMatrix4fv(raster_u_perspective, 1, GL_FALSE, mat);
	
	glUseProgram(surface_raster_program);
	glUniformMatrix4fv(raster_u_perspective, 1, GL_FALSE, mat);
	
	glUseProgram(normal_raster_program);
	glUniformMatrix4fv(raster_u_perspective, 1, GL_FALSE, mat);

	glUseProgram(ray_program);
	glUniform1f(ray_u_fov, fov);
	glUniform1f(ray_u_far, far);
}

void SegmentPathRenderer::set_iterations(int iterations) {
	glUseProgram(ray_program);
	glUniform1i(ray_u_iterations, iterations);
}

void SegmentPathRenderer::draw_raster_framebuffers() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);

	glViewport(0, 0, render_w, render_h);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(raster_vao);

	// Bind material_ubo to binding 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, material_ubo);

	// Draw calls
	// Position, Depth
	glBindFramebuffer(GL_FRAMEBUFFER, position_framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(position_raster_program);
	glDrawArrays(GL_TRIANGLES, 0, raster_num_triangles * 3);
	glFinish();
	// UV, Material, Wall
	glBindFramebuffer(GL_FRAMEBUFFER, surface_framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(surface_raster_program);
	glDrawArrays(GL_TRIANGLES, 0, raster_num_triangles * 3);
	glFinish();
	// Normal
	glBindFramebuffer(GL_FRAMEBUFFER, normal_framebuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(normal_raster_program);
	glDrawArrays(GL_TRIANGLES, 0, raster_num_triangles * 3);
	glFinish();

}

void SegmentPathRenderer::draw_ray_framebuffer() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, position_texture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, surface_texture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normal_texture);

	glViewport(0, 0, render_w, render_h);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Bind segment_ubo to binding 0
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, segment_ubo);
	// Bind material_ubo to binding 1
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, material_ubo);

	glBindVertexArray(ray_vao);
	// Draw call
	glBindFramebuffer(GL_FRAMEBUFFER, ray_framebuffer);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ray_program);
	glDrawArrays(GL_TRIANGLES, 0, ray_num_triangles * 3);
	glFinish();
}

void SegmentPathRenderer::draw_post() {
	// Post-Processing and drawing to screen

	glViewport(0, 0, Window::get_width(), Window::get_height());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ray_texture);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, position_texture);

	// Using same VAO as raytrace
	glBindVertexArray(ray_vao);
	// Draw call
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(post_program);
	glDrawArrays(GL_TRIANGLES, 0, ray_num_triangles * 3);
}

/*
SegmentPathRenderer::SegmentPathRenderer(int num_groups_x, int num_groups_y) {
	ray_program = GL_Utility::load_program("assets/segment_path.vert", "assets/segment_path.frag");

	ray_u_offset		= glGetUniformLocation(program, "u_offset");
	ray_u_rotation		= glGetUniformLocation(program, "u_rotation");
	ray_u_iterations	= glGetUniformLocation(program, "u_iterations");
	ray_u_far			= glGetUniformLocation(program, "u_far");
	ray_u_max_y		= glGetUniformLocation(program, "u_max_y");
	ray_u_min_y		= glGetUniformLocation(program, "u_min_y");

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &ssbo);

	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, 2 * sizeof(float));
	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);

	num_triangles = (num_groups_x * num_groups_y) * 2;

	float group_w = 1.0 / (float)num_groups_x;
	float group_h = 1.0 / (float)num_groups_y;

	glNamedBufferData(vbo, num_triangles * sizeof(float) * 6, NULL, GL_DYNAMIC_DRAW);

	for (int i = 0; i < num_groups_x * num_groups_y; i++) {
		int ix = i % num_groups_x;
		int iy = i / num_groups_x;
		
		float x1 = (group_w * (float)ix) * 2.0 - 1.0;
		float y1 = (group_h * (float)iy) * 2.0 - 1.0;
		float x2 = x1 + group_w * 2.0;
		float y2 = y1 + group_h * 2.0;

		float data[] = {
			x1, y1,
			x1, y2,
			x2, y2,
		
			x1, y1,
			x2, y2,
			x2, y1
		};

		glNamedBufferSubData(vbo, i * sizeof(float) * 12, sizeof(float) * 12, data);
	}
}

SegmentPathRenderer::~SegmentPathRenderer() {

}

//SegmentMaterial* SegmentPathRenderer::add_material() {
//	materials.push_back(SegmentMaterial());
//	return &materials.back();
//}

void SegmentPathRenderer::draw() {
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

	glBindVertexArray(vao);
	glUseProgram(program);
	glDrawArrays(GL_TRIANGLES, 0, num_triangles * 3);
}

void SegmentPathRenderer::set_view_pos(Vec3 v) {
	glProgramUniform3f(program, u_offset, v.x, v.y, v.z);
}
void SegmentPathRenderer::set_view_rotation(float a) {
	glProgramUniform1f(program, u_rotation, a);
}
void SegmentPathRenderer::set_iterations(int iterations) {
	glProgramUniform1i(program, u_iterations, iterations);
}
void SegmentPathRenderer::set_far(float far) {
	glProgramUniform1f(program, u_far, far);
}
void SegmentPathRenderer::set_max_y(float y) {
	glProgramUniform1f(program, u_max_y, y);
}
void SegmentPathRenderer::set_min_y(float y) {
	glProgramUniform1f(program, u_min_y, y);
}

void SegmentPathRenderer::set_segments(std::list<SegmentNode>& list) {
	GLintptr offset = 0;

	glNamedBufferData(ssbo, list.size() * (SegmentNode::SEGMENT_SIZE + SegmentMaterial::MATERIAL_SIZE), NULL, GL_DYNAMIC_DRAW);

	for (std::list<SegmentNode>::iterator it = list.begin(); it != list.end(); ++it) {
		float data[] = {
			it->a.x, it->a.y,
			it->b.x, it->b.y
		};
		
		glNamedBufferSubData(ssbo, offset, SegmentNode::SEGMENT_SIZE, data);
		offset += SegmentNode::SEGMENT_SIZE;

		glNamedBufferSubData(ssbo, offset, sizeof(float), &it->material->diffusion);
		offset += sizeof(float);
		glNamedBufferSubData(ssbo, offset, sizeof(float), &it->material->emission);
		offset += sizeof(float);
	}
}
*/