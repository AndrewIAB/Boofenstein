#version 330 core

// Bind to 0
uniform sampler2D u_texture_ray;
// Bind to 1
uniform sampler2D u_texture_position;

uniform vec2 u_resolution;

in vec2 v_pos;
out vec4 o_color;

vec2 uv;

vec3 get_color(vec2 off) {
	return texture(u_texture_ray, uv + off / u_resolution);
}

float get_brightness(vec2 off) {
	vec3 color = get_color(off);
	return (color.x + color.y + color.z) / 3.0;
}

float get_depth(vec2 off) {
	return texture(u_texture_position, uv + off / u_resolution).w;
}

/*
INTERESTING EFFECT:
	vec3 color;

	float depth_c = get_depth(vec2(0, 0));

	float a = 0.6;
	float b = ((1.0 - a) / 9.0);

	color += get_color(vec2(-1, -1)) * (1.0 + get_depth(vec2(-1, -1)) - depth_c); // * b;
	color += get_color(vec2(-1, +0)) * (1.0 + get_depth(vec2(-1, +0)) - depth_c); // * b;
	color += get_color(vec2(-1, +1)) * (1.0 + get_depth(vec2(-1, +1)) - depth_c); // * b;
	color += get_color(vec2(+0, -1)) * (1.0 + get_depth(vec2(+0, -1)) - depth_c); // * b;
	color += get_color(vec2(+0, +0)) * (1.0 - get_depth(vec2(+0, +0)) - depth_c); // * a;
	color += get_color(vec2(+0, +1)) * (1.0 + get_depth(vec2(+0, +1)) - depth_c); // * b;
	color += get_color(vec2(+1, -1)) * (1.0 + get_depth(vec2(+1, -1)) - depth_c); // * b;
	color += get_color(vec2(+1, +0)) * (1.0 + get_depth(vec2(+1, +0)) - depth_c); // * b;
	color += get_color(vec2(+1, +1)) * (1.0 + get_depth(vec2(+1, +1)) - depth_c); // * b;

	return color / 9.0;
*/

/*
ALSO INTERESTING:
	color += get_color(vec2(-1, -1)) * (1.0 + get_depth(vec2(-1, -1)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(-1, +0)) * (1.0 + get_depth(vec2(-1, +0)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(-1, +1)) * (1.0 + get_depth(vec2(-1, +1)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(+0, -1)) * (1.0 + get_depth(vec2(+0, -1)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(+0, +0)) * (1.0 + get_depth(vec2(+0, +0)) - depth_c) / depth_c; // * a;
	color += get_color(vec2(+0, +1)) * (1.0 + get_depth(vec2(+0, +1)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(+1, -1)) * (1.0 + get_depth(vec2(+1, -1)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(+1, +0)) * (1.0 + get_depth(vec2(+1, +0)) - depth_c) / depth_c; // * b;
	color += get_color(vec2(+1, +1)) * (1.0 + get_depth(vec2(+1, +1)) - depth_c) / depth_c; // * b;
*/

float get_edge(vec2 off) {
	float color;

	float a = +8.0;
	float b = -1.0;

	color += get_depth(vec2(-1, -1) + off) * b;
	color += get_depth(vec2(-1, +0) + off) * b;
	color += get_depth(vec2(-1, +1) + off) * b;
	color += get_depth(vec2(+0, -1) + off) * b;
	color += get_depth(vec2(+0, +0) + off) * a;
	color += get_depth(vec2(+0, +1) + off) * b;
	color += get_depth(vec2(+1, -1) + off) * b;
	color += get_depth(vec2(+1, +0) + off) * b;
	color += get_depth(vec2(+1, +1) + off) * b;

	return color;
}

float get_threshold_edge(vec2 off, float threshold) {
	float edge = get_edge(off);

	return edge >= threshold ? edge : 0;
}

float lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

vec3 lerp3(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}

vec3 get_blur(float val) {
	vec3 color;

	float a = lerp(16.0, 4.0, val);
	float b = lerp(0.0, 2.0, val);
	float c = lerp(0.0, 1.0, val);

	color += get_color(vec2(-1, -1)) * c;
	color += get_color(vec2(-1, +0)) * b;
	color += get_color(vec2(-1, +1)) * c;
	color += get_color(vec2(+0, -1)) * b;
	color += get_color(vec2(+0, +0)) * a;
	color += get_color(vec2(+0, +1)) * b;
	color += get_color(vec2(+1, -1)) * c;
	color += get_color(vec2(+1, +0)) * b;
	color += get_color(vec2(+1, +1)) * c;

	return color / (a + 4 * (b + c));
}

float get_clamp_edge(vec2 point) {
	return max(min(get_edge(point), 1.0), 0.0);
}

float get_blur_brightness(float val) {
	float color;

	float a = lerp(16.0, 4.0, val);
	float b = lerp(0.0, 2.0, val);
	float c = lerp(0.0, 1.0, val);

	color += get_brightness(vec2(-1, -1)) * c;
	color += get_brightness(vec2(-1, +0)) * b;
	color += get_brightness(vec2(-1, +1)) * c;
	color += get_brightness(vec2(+0, -1)) * b;
	color += get_brightness(vec2(+0, +0)) * a;
	color += get_brightness(vec2(+0, +1)) * b;
	color += get_brightness(vec2(+1, -1)) * c;
	color += get_brightness(vec2(+1, +0)) * b;
	color += get_brightness(vec2(+1, +1)) * c;

	return color / (a + 4 * (b + c));
}

float get_blur_edge(float val) {
	vec3 color;

	float a = lerp(16.0, 4.0, val);
	float b = lerp(0.0, 2.0, val);
	float c = lerp(0.0, 1.0, val);

	float b1 = b;
	float b2 = b;
	float b3 = b;
	float b4 = b;
	float c1 = c;
	float c2 = c;
	float c3 = c;
	float c4 = c;

	c1 *= 1.0 - get_clamp_edge(	vec2(-1, -1));
	color += get_color(			vec2(-1, -1)) * c1;

	b1 *= 1.0 - get_clamp_edge(	vec2(-1, +0));
	color += get_color(			vec2(-1, +0)) * b1;

	c2 *= 1.0 - get_clamp_edge(	vec2(-1, +1));
	color += get_color(			vec2(-1, +1)) * c2;

	b2 *= 1.0 - get_clamp_edge(	vec2(+0, -1));
	color += get_color(			vec2(+0, -1)) * b2;

	// Center pixel
	//a *= 1.0 - get_clamp_edge(	vec2(+0, +0))
	color += get_color(			vec2(+0, +0)) * a;

	b3 *= 1.0 - get_clamp_edge(	vec2(+0, +1));
	color += get_color(			vec2(+0, +1)) * b3;

	c3 *= 1.0 - get_clamp_edge(	vec2(+1, -1));
	color += get_color(			vec2(+1, -1)) * c3;

	b4 *= 1.0 - get_clamp_edge(	vec2(+1, +0));
	color += get_color(			vec2(+1, +0)) * b4;

	c4 *= 1.0 - get_clamp_edge(	vec2(+1, +1));
	color += get_color(			vec2(+1, +1)) * c4;

	return color / (a + c1 + c2 + c3 + c4 + b1 + b2 + b3 + b4);
}

vec3 get_sample() {
	float depth = get_depth(vec2(0, 0)) / 5.0;
	depth = max(min(depth, 0.8), 0.25);

	float avg_bright = get_blur_brightness(depth);

	vec3 color = get_color(vec2(0, 0));
	float brightness = get_brightness(vec2(0, 0));

	vec3 pure_color = color / brightness;

	float edge = get_edge(vec2(0, 0));
	edge = min(max(edge, 0.0), 1.0);

	return (pure_color + edge) * avg_bright;
}

void main() {
	uv = v_pos.xy * 0.5 + vec2(0.5, 0.5);

	vec3 sample = get_sample();

	//float edge = get_edge(vec2(0, 0));
	//edge = min(max(edge, 0.0), 1.0);
	float edge = 0;

	o_color = vec4(sample + edge, 1.0);
	//o_color = vec4(vec3(1, 1, 1) * edge, 1.0);
	//o_color = vec4(vec3(1, 1, 1) / depth, 1.0);

	gl_FragDepth = get_depth(vec2(0, 0));
}