#pragma once

#include <string>
#include <vector>
#include <list>
#include "Vec_n.hpp"

class SegmentMaterial {
public:
	static const size_t SIZE		= 15 * sizeof(float);
	static const size_t PADDED_SIZE	= 16 * sizeof(float);

	// 4
	float albedo_texture_layer		= -1;
	float normal_texture_layer		= -1;
	float specular_texture_layer	= -1;
	float emissive_texture_layer	= -1;
	
	// 7
	Vec3 base_albedo = Vec3(0, 0, 0);
	float base_specular = 0;
	Vec3 base_emissive = Vec3(0, 0, 0);
	
	// 4
	Vec2 uv_scale = Vec2(1.0, 1.0);
	Vec2 uv_offset = Vec2(0.0, 0.0);

	inline SegmentMaterial() {}
	
	inline SegmentMaterial(
		int albedo_texture_layer, int normal_texture_layer, int specular_texture_layer, int emissive_texture_layer,
		Vec2 uv_scale, Vec2 uv_offset
	) {
		this->albedo_texture_layer		= (float)albedo_texture_layer;
		this->normal_texture_layer		= (float)normal_texture_layer;
		this->specular_texture_layer	= (float)specular_texture_layer;
		this->emissive_texture_layer	= (float)emissive_texture_layer;

		this->uv_scale = uv_scale;
		this->uv_offset = uv_offset;
	}

	inline SegmentMaterial(
		int albedo_texture_layer, int normal_texture_layer, int specular_texture_layer, int emissive_texture_layer
	) {
		this->albedo_texture_layer = (float)albedo_texture_layer;
		this->normal_texture_layer = (float)normal_texture_layer;
		this->specular_texture_layer = (float)specular_texture_layer;
		this->emissive_texture_layer = (float)emissive_texture_layer;
	}

	inline SegmentMaterial(
		Vec3 albedo, float specular, Vec3 emissive
	) {
		this->base_albedo = albedo;
		this->base_specular = specular;
		this->base_emissive = emissive;

		this->uv_scale = uv_scale;
		this->uv_offset = uv_offset;
	}

	inline SegmentMaterial(
		int albedo_texture_layer, int normal_texture_layer, int specular_texture_layer, int emissive_texture_layer,
		Vec3 albedo, float specular, Vec3 emissive,
		Vec2 uv_scale, Vec2 uv_offset
	) {
		this->albedo_texture_layer = (float)albedo_texture_layer;
		this->normal_texture_layer = (float)normal_texture_layer;
		this->specular_texture_layer = (float)specular_texture_layer;
		this->emissive_texture_layer = (float)emissive_texture_layer;

		this->base_albedo = albedo;
		this->base_specular = specular;
		this->base_emissive = emissive;

		this->uv_scale = uv_scale;
		this->uv_offset = uv_offset;
	}

	inline SegmentMaterial(
		int albedo_texture_layer, int normal_texture_layer, int specular_texture_layer, int emissive_texture_layer,
		Vec3 albedo, float specular, Vec3 emissive
	) {
		this->albedo_texture_layer = (float)albedo_texture_layer;
		this->normal_texture_layer = (float)normal_texture_layer;
		this->specular_texture_layer = (float)specular_texture_layer;
		this->emissive_texture_layer = (float)emissive_texture_layer;

		this->base_albedo = albedo;
		this->base_specular = specular;
		this->base_emissive = emissive;
	}

	inline SegmentMaterial(
		int albedo_texture_layer,
		float specular, Vec3 emissive
	) {
		this->albedo_texture_layer = (float)albedo_texture_layer;
		
		this->base_specular = specular;
		this->base_emissive = emissive;
	}
};

class SegmentNode {
public:
	static const size_t SIZE		= 5 * sizeof(float);
	static const size_t PADDED_SIZE = 8 * sizeof(float);

	// 4
	Vec2 a;
	Vec2 b;

	// 1
	float material_id;

	inline SegmentNode(Vec2 a, Vec2 b, int material) {
		this->a = a; this->b = b; this->material_id = (float)material;
	}
};

class SegmentPathRenderer {
private:
	// Position, Normal, Tangent, Bitangent, UV, Material ID, Wall ID
	// 3, 3, 3, 3, 2, 1, 1
	const static size_t DATA_PER_VERT = 16 * sizeof(float);

	unsigned int segment_ubo = 0;
	unsigned int material_ubo = 0;

	unsigned int num_textures = 0;
	unsigned int texture_array = 0;

	//unsigned int color_framebuffer	= 0;
	//unsigned int normal_framebuffer	= 0;
	//unsigned int wall_framebuffer	= 0;
	//unsigned int depth_framebuffer	= 0;
	unsigned int position_framebuffer	= 0;
	unsigned int surface_framebuffer	= 0;
	unsigned int normal_framebuffer		= 0;
	unsigned int ray_framebuffer		= 0;

	unsigned int depth_buffer_texture = 0;
	//unsigned int color_texture	= 0;
	//unsigned int normal_texture	= 0;
	//unsigned int wall_texture	= 0;
	//unsigned int depth_texture	= 0;
	unsigned int position_texture = 0;
	unsigned int surface_texture = 0;
	unsigned int normal_texture = 0;
	unsigned int ray_texture	= 0;

	// Raster Objects
	unsigned int raster_vao = 0;
	unsigned int raster_vbo = 0;
	unsigned int position_raster_program	= 0;
	unsigned int surface_raster_program		= 0;
	unsigned int normal_raster_program		= 0;
	//unsigned int color_raster_program	= 0;
	//unsigned int normal_raster_program	= 0;
	//unsigned int wall_raster_program	= 0;
	//unsigned int depth_raster_program	= 0;
	unsigned int raster_num_triangles	= 0;
	// Raster Shader Uniforms
	unsigned int raster_u_offset = 0;
	unsigned int raster_u_rotation = 1;
	unsigned int raster_u_perspective = 2;

	// Ray Objects
	unsigned int ray_vao = 0;
	unsigned int ray_vbo = 0;
	unsigned int ray_program = 0;
	unsigned int ray_num_triangles = 0;
	// Ray Shader Uniforms
	unsigned int ray_u_offset = 0;
	unsigned int ray_u_rotation = 0;
	unsigned int ray_u_iterations = 0;
	unsigned int ray_u_far = 0;
	unsigned int ray_u_fov = 0;
	unsigned int ray_u_max_y = 0;
	unsigned int ray_u_min_y = 0;

	// Post-Processing reuses vertex data from raytrace :3
	unsigned int post_program = 0;

	int render_w;
	int render_h;
public:
	SegmentPathRenderer(int render_w, int render_h, int num_groups_x, int num_groups_y, std::string asset_path);
	~SegmentPathRenderer();

	void draw_raster_framebuffers();
	void draw_ray_framebuffer();
	void draw_post();
	void draw() { draw_raster_framebuffers(); draw_ray_framebuffer(); draw_post();  }

	void set_view_pos(Vec3 v);
	void set_view_rotation(float a);
	
	void set_perspective(float near, float far, float fov);

	void set_iterations(int iterations);

	void set_materials(std::vector<SegmentMaterial>& material_list);
	void set_segments(
		std::list<SegmentNode>& segment_list, std::vector<SegmentMaterial>& materials,
		float max_y, float min_y, int roof_material_id, int floor_material_id);
};