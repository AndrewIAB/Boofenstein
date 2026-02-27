#version 330 core

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
	vec2 uv = fract(pix_uv_coord.xy);
	o_color = vec4(uv, pix_material_id, pix_wall_id);
}