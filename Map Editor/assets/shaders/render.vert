#version 330 core

uniform mat4 u_camera_projection;
uniform mat4 u_camera_transform;
uniform mat4 u_world_transform;
uniform mat3 u_uv_transform;

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;

void main() {
	mat4 point_transform = u_camera_projection * u_camera_transform * u_world_transform;
	vec4 point = point_transform * vec4(a_pos, 1.0);

	vec3 t_uv = u_uv_transform * vec3(a_uv, 1.0);
	
	v_uv = t_uv.xy;

	gl_Position = point;
}