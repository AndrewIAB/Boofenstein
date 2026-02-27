#include "EditorRenderer.hpp"

#include <cmath>

#include "RendererBase.hpp"

#include "EditorMap.hpp"
#include "EditorInterface.hpp"
#include "Vec_n.hpp"

constexpr float PI_F = (float)3.1415926535;
constexpr float PI_D = (double)3.1415926535;

EditorRenderer::EditorRenderer() {
	float point_data[] = {
		0, 0, 0,	0, 0
	};

	float plane_data[] = {
		0, 0, 0,	0, 0,
		1, 0, 0,	1, 0,
		1, 1, 0,	1, 1,

		0, 0, 0,	0, 0,
		1, 1, 0,	1, 1,
		0, 1, 0,	0, 1
	};

	float grid_bounds = 100;

	float xz_quad_data[] = {
		-grid_bounds, 0, -grid_bounds,	-grid_bounds, -grid_bounds,
		+grid_bounds, 0, -grid_bounds,	+grid_bounds, -grid_bounds,
		+grid_bounds, 0, +grid_bounds,	+grid_bounds, +grid_bounds,

		-grid_bounds, 0, -grid_bounds,	-grid_bounds, -grid_bounds,
		+grid_bounds, 0, +grid_bounds,	+grid_bounds, +grid_bounds,
		-grid_bounds, 0, +grid_bounds,	-grid_bounds, +grid_bounds
	};

	renderer.alloc_buffers(sizeof(point_data) + sizeof(plane_data) + sizeof(xz_quad_data));
	point_model = renderer.push_model(point_data, sizeof(point_data), sizeof(point_data) / sizeof(float) / 5);
	plane_model = renderer.push_model(plane_data, sizeof(plane_data), sizeof(plane_data) / sizeof(float) / 5);
	xz_quad_model = renderer.push_model(xz_quad_data, sizeof(xz_quad_data), sizeof(xz_quad_data) / sizeof(float) / 5);

	renderer.add_vertex_attribute({ 3, RTYPE_FLOAT, false }); // Position
	renderer.add_vertex_attribute({ 2, RTYPE_FLOAT, false }); // UV
	renderer.apply_vertex_attributes();

	shader = renderer.load_shader("assets/shaders/render.vert", "assets/shaders/render.frag");

	grid_texture = renderer.load_texture("assets/grid.bmp", false);
	cat_texture = renderer.load_texture("assets/car.jpg", false);
	
	u_camera_projection	= renderer.get_uniform(shader, "u_camera_projection");
	u_camera_transform	= renderer.get_uniform(shader, "u_camera_transform");
	u_world_transform	= renderer.get_uniform(shader, "u_world_transform");
	u_uv_transform		= renderer.get_uniform(shader, "u_uv_transform");

	u_texture = renderer.get_uniform(shader, "u_texture");

	// Setting texture unit
	renderer.set_uniform(shader, u_texture, (int)0);
}

EditorRenderer::~EditorRenderer() {
	// Um uhh hummm hurr durr
	// RendererBase will free all data it uses
	// SOoooo ye
	// HACK: Might need to update this later on
}

void EditorRenderer::set_perspective(float fov_degrees, float far, float near, float aspect_ratio) {
	float fov_rad = 180.0 / PI_F * fov_degrees;

	const float t_inv = aspect_ratio / (tanf(fov_rad / 2));
	const float fn_d = far - near;
	const float f_n = far + near;
	const float fn2 = far * near * 2;

	const float f_n_d = f_n / fn_d;
	const float fn2_d = -fn2 / fn_d;

	proj_perspective.r1 = Vec4(t_inv,		0,		0,		0		);
	proj_perspective.r2 = Vec4(0,			t_inv,	0,		0		);
	proj_perspective.r3 = Vec4(0,			0,		f_n_d,	fn2_d	);
	proj_perspective.r4 = Vec4(0,			0,		1,		0		);
}

void EditorRenderer::set_orthograpic(float scale, float aspect_ratio) {
	float s = scale * aspect_ratio;

	proj_orthographic.r1 = Vec4(s,	0,	0,	0);
	proj_orthographic.r2 = Vec4(0,	0,	s,	0);
	proj_orthographic.r3 = Vec4(0,	1,	0,	0);
	proj_orthographic.r4 = Vec4(0,	0,	0,	1);
}

void EditorRenderer::set_view_offset(Vec2 offset, float height) {
	// Inverse of offset
	camera_translate.r1.w = -offset.x; // Translate X
	camera_translate.r2.w = -height;   // Translate Y
	camera_translate.r3.w = -offset.y; // Translate Z
}

void EditorRenderer::set_view_rotation(float rotation) {
	// Inverse of rotation
	float c = cos(-rotation);
	float s = sin(-rotation);

	// Normal, Bitangent, Tangent
	Vec4 t = Vec4(c,	0,	s,	0);
	Vec4 b = Vec4(0,	1,	0,	0);
	Vec4 n = Vec4(-s,	0,	c,	0);

	camera_orthogonal.r1 = t;
	camera_orthogonal.r2 = b;
	camera_orthogonal.r3 = n;
	camera_orthogonal.r4 = Vec4(0, 0, 0, 1);
}

void EditorRenderer::set_view_transform() {
	camera_transform = camera_orthogonal * camera_translate;
}

/*
Mat4 gen_quad_matrix() {
	
}
*/

void EditorRenderer::draw_test() {
	renderer.enable_depth_test();

	renderer.set_uniform(shader, u_camera_projection,	proj_perspective);
	renderer.set_uniform(shader, u_camera_transform,	camera_transform);
	
	// Draw grid
	Vec3 camera_pos = Vec3(-camera_translate.r1.w, -camera_translate.r2.w, -camera_translate.r3.w);
	Vec3 grid_pos = Vec3((int)camera_pos.x, 0, (int)camera_pos.z);
	renderer.set_uniform(shader, u_world_transform,		Mat4::translate(grid_pos));
	renderer.set_uniform(shader, u_uv_transform,		Mat3::identity());
	renderer.bind_texture_2D(grid_texture, 0);
	renderer.draw_triangles(shader, xz_quad_model);

	renderer.bind_texture_2D(cat_texture, 0);
	renderer.set_uniform(shader, u_world_transform, Mat4::identity());
	renderer.set_uniform(shader, u_uv_transform, Mat3::identity());
	renderer.draw_triangles(shader, plane_model);
}