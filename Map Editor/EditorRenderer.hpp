#pragma once

#include "RendererBase.hpp"
#include "EditorMap.hpp"
#include "EditorInterface.hpp"
#include "Vec_n.hpp"

class EditorRenderer {
private:
	RendererBase renderer;

	RModel point_model;		// Used for selecting individual points
	RModel plane_model;		// Used for every wall/floor/ceiling
	RModel xz_quad_model;	// Spans across the xy axis as a grid

	RID shader;

	RID grid_texture;
	RID cat_texture;
	
	// Matrices
	RID u_camera_projection;
	RID u_camera_transform;
	RID u_world_transform;
	RID u_uv_transform;
	
	RID u_texture;

	Mat4 camera_translate	= Mat4::identity();	// Position of camera
	Mat4 camera_orthogonal	= Mat4::identity();	// Rotation of camera
	Mat4 camera_transform	= Mat4::identity();	// Translate + Orthogonal

	Mat4 proj_perspective	= Mat4::identity();	// Perspective / 3D projection
	Mat4 proj_orthographic	= Mat4::identity();	// Orthographic / 2D projection
public:
	EditorRenderer();
	~EditorRenderer();

	// Generate new perspective mat
	void set_perspective(float fov_degrees, float far, float near, float aspect_ratio);
	void set_orthograpic(float scale, float aspect_ratio);

	void set_view_offset(Vec2 offset, float height);
	void set_view_rotation(float rotation);
	// Apply offset and rotation to camera_transform_mat
	void set_view_transform();
	
	void draw_test();

	void draw_2D(const EditorMap& map, const EditorInterface::EditorState& editor_state);
	void draw_3D(const EditorMap& map, const EditorInterface::EditorState& editor_state);
};