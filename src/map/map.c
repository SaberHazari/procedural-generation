#include "map.h"
#include <math.h>

static void generate_noise_map(MapInfo *map_info, f32 *noise_map) {
    if(map_info->scale <= 0.0f) { map_info->scale = 0.0001f; }
    if(map_info->octaves > MAX_OCTAVES) { 
        map_info->octaves = MAX_OCTAVES; 
    }
    
    f32 oct_x[map_info->octaves], oct_y[map_info->octaves];
    Prng prng;
    prng_seed(&prng, map_info->seed);
    
    for(i32 i = 0; i < map_info->octaves; ++i) {
        oct_x[i] = prng_range(&prng, -100000.0f, 100000.0f) + map_info->offset_x;
        oct_y[i] = prng_range(&prng, -100000.0f, 100000.0f) + map_info->offset_y;
    }
    
    f32 half_w = (f32)map_info->map_w * 0.5f;
    f32 half_h = (f32)map_info->map_h * 0.5f;
    f32 min_h = 1e30f, max_h = -1e30f;
    
    for(i32 y = 0; y < map_info->map_h; ++y) {
        for(i32 x = 0; x < map_info->map_w; ++x) {
            f32 amplitude = 1.0f, frequency = 1.0f, h = 0.0f;
            
            for(i32 i = 0; i < map_info->octaves; ++i) {
                f32 sx = ((f32)x - half_w) / map_info->scale * frequency + oct_x[i];
                f32 sy = ((f32)y - half_h) / map_info->scale * frequency + oct_y[i];
                h += perlin2d(sx, sy) * amplitude;
                amplitude *= map_info->persistence;
                frequency *= map_info->lacunarity;
            }
            noise_map[y * map_info->map_w + x] = h;
            if(h < min_h) { min_h = h; }
            if(h > max_h) { max_h = h; }
        }
    }
    
    f32 range = max_h - min_h;
    if(range < 1e-6f) { range = 1e-6f; }
    for(i32 i = 0; i < (map_info->map_w * map_info->map_h); ++i) {
        noise_map[i] = (noise_map[i] - min_h) / range;
    }
}

static const CellInfo NOISE_GRADIENT[] = {
    { 0.00f, { 0.00f, 0.00f, 0.00f } }, 
    { 0.25f, { 0.25f, 0.25f, 0.25f } }, 
    { 0.50f, { 0.50f, 0.50f, 0.50f } }, 
    { 0.75f, { 0.75f, 0.75f, 0.75f } }, 
    { 1.00f, { 1.00f, 1.00f, 1.00f } }, 
};

static u8 noise_index_for_height(f32 h) {
    i32 count = array_count(NOISE_GRADIENT);
    for(i32 i = 0; i < count; ++i) {
        if(h <= NOISE_GRADIENT[i].height) { return (u8)i; }
    }
    return (u8)(count - 1);
}

static inline f32 falloff_curve(f32 v) {
    const f32 a = 3.0f;
    const f32 b = 2.2f;
    f32 va = powf(v, a);
    f32 vb = powf(b - b * v, a);
    return va / (va + vb);
}

static inline f32 falloff_value(i32 x, i32 y, i32 width, i32 height) {
    f32 nx = (f32)x / (f32)width * 2.0f - 1.0f;
    f32 ny = (f32)y / (f32)height * 2.0f - 1.0f;
    f32 v = fmaxf(fabsf(nx), fabsf(ny));
    if(v > 1.0f) { v = 1.0f; }
    return falloff_curve(v);
}

u8 *build_map(MapInfo *map_info) {
    f32 *noise_map = (f32 *)calloc((size_t)(map_info->map_w * map_info->map_h), 
        sizeof(f32));
    generate_noise_map(map_info, noise_map);
    u8 *map = (u8 *)calloc((size_t)(map_info->map_w * map_info->map_h), 
        sizeof(u8));
    for(i32 y = 0; y < map_info->map_h; ++y) {
        for(i32 x = 0; x < map_info->map_w; ++x) {
            i32 idx = y * map_info->map_w + x;
            f32 h = noise_map[idx] - falloff_value(x, y, 
                map_info->map_w, map_info->map_h);
            if(h < 0.0f) { h = 0.0f; }
            if(h > 1.0f) { h = 1.0f; }
            map[idx] = noise_index_for_height(h);
        }
    }
    
    free(noise_map);
    return map;
}

static inline u8 color_f32_to_u8(f32 color) {
    u8 result = (u8)(color * 255.0f);
    return result;
}

void render_viewport(MapInfo *map_info, const u8 *map, 
    u8 *image) {
    for(i32 y = 0; y < map_info->viewport_h; ++y) {
        for(i32 x = 0; x < map_info->viewport_w; ++x) {
            u8 cell = map[((i32)map_info->offset_y + y) * 
                map_info->map_w + ((i32)map_info->offset_x + x)];
            Vec3 c = NOISE_GRADIENT[cell].color;
            i32 idx = (y * map_info->viewport_w + x) * 3;
            image[idx + 0] = color_f32_to_u8(c.x);
            image[idx + 1] = color_f32_to_u8(c.y);
            image[idx + 2] = color_f32_to_u8(c.z);
        }
    }
}