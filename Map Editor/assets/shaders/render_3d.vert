#version 330 core

layout (location = 0) in vec2 a_pos;

uniform mat3 u_translation_mat;
uniform mat3 u_orthogonal_mat;
uniform float u_aspect_ratio;

void main() {
	mat3 matrix = u_translation_mat * u_orthogonal_mat;
	vec3 vert = matrix * vec3(a_pos, 1.0);
	vert.x /= u_aspect_ratio;

	gl_Position = vec4(vert.xy / vert.z, 0.0, 1.0);
}