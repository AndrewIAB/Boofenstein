#version 330 core

layout (location = 0) in vec2 a_position;

out vec2 v_pos;

void main() {
	v_pos = a_position.xy;
	gl_Position = vec4(a_position.xy, 0.0, 1.0);
}