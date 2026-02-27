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

struct WallSegment {
	// :P implementations oft get the padding wrong

	// 4
	vec2 a;
	vec2 b;

	// 1
	float material_id;

	// 3
	float padding0;
};

struct RayResult {
	int wall; // -1 is none, -2 is roof, -3 is floor, >= 0 is wall
	vec3 hit;
	vec3 normal;
	vec2 uv;
};

#define MAX_SEGMENTS 1024
#define MAX_MATERIALS 512

// Bind to 0
layout (std140) uniform ub_Segments {
	WallSegment segment_data[MAX_SEGMENTS];
};

// Bind to 1
layout (std140) uniform ub_Materials {
	Material materials_data[MAX_MATERIALS];
};

// Add outside uniform block to avoid padding issues and because im lazyy :3
uniform int u_num_segments;

//// Bind to 0
//uniform sampler2DArray	u_texture_array;
//// Bind to 1
//uniform sampler2D		u_texture_color;
//// Bind to 2
//uniform sampler2D		u_texture_normal;
//// Bind to 3
//uniform sampler2D		u_texture_wall;

// Bind to 0
uniform sampler2DArray	u_texture_array;
// Bind to 1
uniform sampler2D		u_texture_position;
// Bind to 2
uniform sampler2D		u_texture_surface;
// Bind to 3
uniform sampler2D		u_texture_normal;

uniform vec3 u_offset;
uniform float u_rotation;

uniform int u_iterations;

uniform float u_far;
uniform float u_fov;
uniform float u_max_y;
uniform float u_min_y;

uniform int u_roof_material_id;
uniform int u_floor_material_id;

in vec2 v_pos;
out vec4 o_color;

float PHI = 1.61803398874989484820459;  // Golden Ratio

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

float rand_seed = 0;
float rand() {
	float a = gold_noise(v_pos * 0.5 + 0.5 + 100, rand_seed);
	a = abs(a) * 2.0 - 1.0;

	rand_seed += a;
	//rand_seed = mod(rand_seed, 1e5) + 1e4;

	return a;
}

vec3 rand3() {
	return vec3(rand(), rand(), rand());
}

vec3 lerp3(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}

vec2 get_segment_normal(WallSegment seg) {
	vec2 delta = seg.b - seg.a;
	return vec2(delta.y, -delta.x);
}

vec3 get_y_point(vec3 dir, vec3 origin, float far, float height) {
	if (dir.y == 0) {
		return dir * far;
	}

	vec3 hit = dir * (height - origin.y) / dir.y;

	if (length(hit) > far) {
		return dir * far;
	}

	return hit + vec3(origin.x, origin.y, origin.z);
}

vec2 segment_collision(vec2 a, vec2 b, vec2 c, vec2 d) {
	float coef = 1.0 / ((a.x - b.x) * (c.y - d.y) - (a.y - b.y) * (c.x - d.x));

	float t = ((a.x - c.x) * (c.y - d.y) - (a.y - c.y) * (c.x - d.x)) * coef;
	float u = ((a.x - b.x) * (a.y - c.y) - (a.y - b.y) * (a.x - c.x)) * -coef;

	return vec2(t, u); 
}

vec2 get_segment_pos(vec2 a, vec2 b, float t) {
	return a + (b - a) * t;
}

vec2 translate_point(vec2 point, vec2 offset, float rotation) {
	float c = cos(rotation);
	float s = sin(rotation);
	vec2 p1 = point - offset;
	vec2 p2 = vec2(p1.x * c - p1.y * s, p1.y * c + p1.x * s);
	return p2;
}

vec3 rotate_point(vec3 point, float rotation) {
	float c = cos(rotation);
	float s = sin(rotation);
	return vec3(point.x * c + point.z * s, point.y, point.z * c - point.x * s);
}

vec3 get_world_point(vec3 point, vec3 offset, float rotation) {
	float c = cos(-rotation);
	float s = sin(-rotation);
	vec2 r_xz = vec2(point.x * c - point.z * s, point.z * c + point.x * s);

	return vec3(r_xz.x, point.y, r_xz.y) + offset;
}

Material get_ray_material(RayResult ray) {
	Material mat;
	
	if (ray.wall == -2) {
		mat = materials_data[u_roof_material_id];
	} else
	if (ray.wall == -3) {
		mat = materials_data[u_floor_material_id];
	} else {
		mat = materials_data[int(segment_data[ray.wall].material_id)];
	}

	return mat;
}

vec3 get_wall_tangent(vec3 normal) {
	if (normal.y == 0) {
		return vec3(0, 1, 0);
	} else {
		return vec3(1, 0, 0);
	}
}

vec3 get_wall_bitangent(vec3 normal) {
	if (normal.y == 0) {
		return vec3(-normal.y, 0.0, normal.x);
	} else {
		return vec3(0, 0, 1);
	}
}

vec3 sample_wall_normal(float layer, vec2 uv) {
	if (layer == -1) {
		return vec3(0, 0, 1);
	}
	return texture2DArray(u_texture_array, vec3(uv, layer)).xyz * 2.0 - 1.0;
}

RayResult get_ray(vec3 origin, float rotation, vec3 dir, float far, int ignore_wall) {
	RayResult ray;

	ray.hit = origin + dir * far;
	
	vec2 dir_xz = vec2(dir.x, dir.z);
	vec2 origin_xz = vec2(origin.x, origin.z);

	int i = 0;
	vec2 col;

	while (i < u_num_segments) {
		if (i == ignore_wall) {
			i++;
			continue;
		}

		WallSegment wall = segment_data[i];

		vec2 hit_xz = vec2(ray.hit.x, ray.hit.z);
		vec2 curr_col = segment_collision(origin_xz, hit_xz, wall.a, wall.b);

		// Test if curr_col is within [0, 1)
		bool test_col_0	= floor(curr_col.x) != 0;
		bool test_col_1	= floor(curr_col.y) != 0;
		// Test if curr_col is facing correct direction
		bool test_order	= dot(get_segment_normal(wall), dir_xz) > 0.0;

		if (test_col_0 || test_col_1 || test_order) {
			i++;
			continue;
		}

		//if (curr_col.x < 0 || curr_col.x > 1 || curr_col.y < 0 || curr_col.y > 1) {
		//	i++;
		//	continue;
		//}

		col = curr_col;
		ray.wall = i;
		hit_xz = get_segment_pos(wall.a, wall.b, col.y);
		ray.hit = vec3(hit_xz.x, 0.0, hit_xz.y);

		i++;
	}

	WallSegment wall = segment_data[ray.wall];

	vec3 abs_dir = rotate_point(dir, -rotation);
	vec2 rel_point_xz = get_segment_pos(vec2(0, 0), vec2(abs_dir.x, abs_dir.z) * far, col.x);
	ray.hit.y = abs_dir.y * rel_point_xz.y / abs_dir.z + origin.y;

	//vec3 rel_hit = ray.hit - origin;
	//ray.hit.y = rel_hit.x * dir.y / dir.x - rel_hit.y;

	if (ignore_wall != -3 && ray.hit.y < u_min_y) {
		ray.wall = -3;
		ray.hit = get_y_point(dir, origin, far, u_min_y);
	} else
	if (ignore_wall != -2 && ray.hit.y > u_max_y) {
		ray.wall = -2;
		ray.hit = get_y_point(dir, origin, far, u_max_y);
	}

	if (ray.wall >= 0) {
		ray.uv.x = col.y * distance(wall.a, wall.b);
		ray.uv.y = ray.hit.y - (u_min_y);

		vec2 normal_xz = get_segment_normal(wall);

		ray.normal.x = normal_xz.x;
		ray.normal.z = normal_xz.y;

		ray.normal = normalize(ray.normal);
	} else {
		ray.uv.x = ray.hit.x;
		ray.uv.y = ray.hit.z;

		ray.normal.y = ray.wall == -2 ? -1 : 1;
	}
	
	Material mat = get_ray_material(ray);

	ray.uv.x *= mat.uv_scale_x;
	ray.uv.y *= mat.uv_scale_y;
	ray.uv.x += mat.uv_offset_x;
	ray.uv.y += mat.uv_offset_y;
	// Basically same as mod 1
	ray.uv = fract(ray.uv);
	
	mat3 tbn = mat3(get_wall_tangent(ray.normal), get_wall_bitangent(ray.normal), ray.normal);
	vec3 sample_normal = sample_wall_normal(mat.normal_texture_layer, ray.uv);
	ray.normal = tbn * sample_normal;

	return ray;
}

vec3 get_material_albedo(Material material, vec2 uv) {
	if (material.albedo_texture_layer == -1) {
		return material.base_albedo;
	}

	return texture2DArray(u_texture_array, vec3(uv, material.albedo_texture_layer)).xyz + material.base_albedo;
}

float get_material_specular(Material material, vec2 uv) {
	if (material.specular_texture_layer == -1) {
		return material.base_specular;
	}
	
	return texture2DArray(u_texture_array, vec3(uv, material.specular_texture_layer)).x + material.base_specular;
}

vec3 get_material_emissive(Material material, vec2 uv) {
	if (material.emissive_texture_layer == -1) {
		return material.base_emissive;
	}

	return texture2DArray(u_texture_array, vec3(uv, material.emissive_texture_layer)).xyz + material.base_emissive;
}

#define NUM_REFLECTIONS 2
#define NUM_SAMPLES 15

vec3 sample_ray(Material first_material, vec2 first_uv, vec3 origin, float rotation, vec3 first_normal, vec3 first_dir, float far, int ignore_wall) {
	vec3 dir = first_dir;

	RayResult ray;
	ray.hit = origin;
	ray.wall = ignore_wall;
	ray.normal = first_normal;
	ray.uv = first_uv;

	Material material = first_material;

	vec3 color = get_material_emissive(material, ray.uv) * get_material_albedo(material, ray.uv);
	vec3 surface_color = get_material_albedo(material, ray.uv);
	
	for (int i = 0; i < NUM_REFLECTIONS; i++) {
		float grid = int(mod(gl_FragCoord.x, 2)) == int(mod(gl_FragCoord.y, 2)) ? 1 : -1;
		
		vec3 diffuse_dir = ray.normal + rand3() * grid;
		vec3 specular_dir = reflect(dir, ray.normal);
		dir = normalize(lerp3(diffuse_dir, specular_dir, get_material_specular(material, ray.uv)));
		
		ray = get_ray(ray.hit, rotation, dir, far, ray.wall);

		material = get_ray_material(ray);

		color += get_material_emissive(material, ray.uv) * surface_color;
		surface_color *= get_material_albedo(material, ray.uv);
	}

	return color;
}

void main() {
	vec2 uv = v_pos.xy * 0.5 + vec2(0.5, 0.5);

	vec4 tex_position	= texture2D(u_texture_position,	uv);
	vec4 tex_surface	= texture2D(u_texture_surface,	uv);
	vec4 tex_normal		= texture2D(u_texture_normal,	uv);

	vec3 pix_position		= tex_position.xyz;
	vec3 pix_normal			= tex_normal.xyz;
	vec2 pix_uv				= tex_surface.xy;
	Material pix_material	= materials_data[int(tex_surface.z)];
	int pix_wall			= int(tex_surface.w);
	
	float lense_dist = 1.0 / tan(u_fov * 0.5);
	vec3 dir = vec3(v_pos.xy, lense_dist);
	dir = normalize(dir);
	dir = rotate_point(dir, u_rotation);

	vec3 ray_color;

	int i = 0;

	float coeff = 1.0 / float(NUM_SAMPLES);

	for (int i = 0; i < NUM_SAMPLES; ++i) {
		ray_color += sample_ray(pix_material, pix_uv, pix_position, u_rotation, pix_normal, dir, u_far, pix_wall) * coeff;
	}

	o_color = vec4(ray_color.xyz, 1.0);
	
	//o_color = vec4(pix_normal, 1.0);

	//o_color = vec4(abs(rand3()), 1.0);
}

//RayResult ray = get_ray(u_offset, u_rotation, dir, u_far, -1);

//RayResult ray = get_ray(pix_position, u_rotation, dir, u_far, -1);
//vec4 material = get_ray_material(ray);
//vec4 ray_color = texture2DArray(u_texture_array, vec3(ray.uv.xy, material.x));

//o_color = vec4(ray_color.xyz, 1.0);
//o_color = vec4(pix_color.xyz, 1.0);