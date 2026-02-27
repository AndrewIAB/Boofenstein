#version 330 core


struct Material {
	// 4
	float albedo_texture_layer;
	float normal_texture_layer;
	float specular_texture_layer;
	float emissive_texture_layer;
	
	// 7
	vec3	base_albedo;
	float	base_specular;
	vec3	base_emissive;

	// 4
	// Was having padding issues -w-
	float uv_scale_x;
	float uv_scale_y;
	float uv_offset_x;
	float uv_offset_y;

	// 1
	float padding0;
};

#define MAX_MATERIALS 512

// Bind to 0
layout (std140) uniform ub_Materials {
	Material materials_data[MAX_MATERIALS];
};


// Bind to 0
uniform sampler2DArray u_texture_array;

in vec3		pix_normal;
in vec2		pix_uv_coord;
in vec3		pix_pos;
in float	pix_wall_id;
in float	pix_material_id;
in float	pix_depth;
in mat3		tbn_mat;

out vec4 o_color;

void main() {
	Material mat = materials_data[int(pix_material_id)];

	vec3 normal = pix_normal;

	if (mat.normal_texture_layer >= 0) {
		normal = texture2DArray(u_texture_array, vec3(fract(pix_uv_coord), mat.normal_texture_layer)).xyz * 2.0 - 1.0;
		normal = normalize(tbn_mat * normal);
	}

	o_color = vec4(normal.xyz, 0.0);
}