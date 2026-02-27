#version 330 core

uniform sampler2D u_texture;

in vec2 v_uv;
out vec4 o_color;

void main() {
	vec2 uv = fract(v_uv);

	vec4 texture_color = texture(u_texture, uv.xy);

	if (texture_color.w < 0.5) {
		discard;
	}

	o_color = vec4(texture_color.xyz, 1.0);
	//o_color = vec4(uv, 0.0, 1.0);
}