#version 330 core

layout (location = 0) in vec3	a_position;
layout (location = 1) in vec3	a_normal;
layout (location = 2) in vec3	a_tangent;
layout (location = 3) in vec3	a_bitangent;
layout (location = 4) in vec2	a_uv_coord;
layout (location = 5) in float	a_material_id;
layout (location = 6) in float	a_wall_id;

out vec3	pix_normal;
out vec2	pix_uv_coord;
out vec3	pix_pos;
out float	pix_wall_id;
out float	pix_material_id;
out float	pix_depth;
out mat3	tbn_mat;

uniform vec3 u_offset;
uniform float u_rotation;
uniform mat4 u_perspective;

vec2 translate_point_2d(vec2 point, vec2 offset, float rotation) {
	float c = cos(rotation);
	float s = sin(rotation);
	vec2 p1 = point - offset;
	vec2 p2 = vec2(p1.x * c - p1.y * s, p1.y * c + p1.x * s);
	return p2;
}

vec3 translate_point(vec3 point, vec3 offset, float rotation) {
	vec2 v = translate_point_2d(vec2(point.x, point.z), vec2(offset.x, offset.z), rotation);
	return vec3(v.x, point.y - offset.y, v.y);
}

vec3 get_world_point(vec3 point, vec3 offset, float rotation) {
	float c = cos(-rotation);
	float s = sin(-rotation);
	vec2 r_xz = vec2(point.x * c - point.z * s, point.z * c + point.x * s);

	return vec3(r_xz.x, point.y, r_xz.y) + offset;
}

void main() {
	vec3 pos = translate_point(a_position, u_offset, u_rotation);
	vec4 perspective_pos = vec4(pos, 1.0) * u_perspective;
	
	//pix_normal = translate_point(a_normal, vec3(0.0, 0.0, 0.0), u_rotation);
	pix_normal = a_normal;
	pix_uv_coord = a_uv_coord;
	pix_pos = a_position.xyz;
	pix_wall_id = a_wall_id;
	pix_material_id = a_material_id;
	pix_depth = perspective_pos.z;

	tbn_mat = mat3(a_tangent, a_bitangent, a_normal);

	gl_Position = perspective_pos;
}