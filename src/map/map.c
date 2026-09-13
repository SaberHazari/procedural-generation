#include "map.h"

void generate_noise_map(f32 *noise_map, i32 width, 
    i32 height, u32 seed, f32 scale, i32 octaves, 
    f32 persistence, f32 lacunarity, f32 offset_x, 
    f32 offset_y) {
    if(scale <= 0.0f) { scale = 0.0001f; }
    if(octaves > MAX_OCTAVES) { octaves = MAX_OCTAVES; }
    
    f32 oct_x[octaves], oct_y[octaves];
    Prng prng;
    prng_seed(&prng, seed);
    
    for(i32 i = 0; i < octaves; ++i) {
        oct_x[i] = prng_range(&prng, -100000.0f, 100000.0f) + offset_x;
        oct_y[i] = prng_range(&prng, -100000.0f, 100000.0f) + offset_y;
    }
    
    f32 half_w = (f32)width * 0.5f;
    f32 half_h = (f32)height * 0.5f;
    f32 min_h = 1e30f, max_h = -1e30f;
    
    for(i32 y = 0; y < height; ++y) {
        for(i32 x = 0; x < width; ++x) {
            f32 amplitude = 1.0f, frequency = 1.0f, h = 0.0f;
            
            for(i32 i = 0; i < octaves; ++i) {
                f32 sx = ((f32)x - half_w) / scale * frequency + oct_x[i];
                f32 sy = ((f32)y - half_h) / scale * frequency + oct_y[i];
                h += perlin2d(sx, sy) * amplitude;
                amplitude *= persistence;
                frequency *= lacunarity;
            }
            noise_map[(i64)y * (i64)width + (i64)x] = h;
            if(h < min_h) { min_h = h; }
            if(h > max_h) { max_h = h; }
        }
    }
    
    f32 range = max_h - min_h;
    if(range < 1e-6f) { range = 1e-6f; }
    for(i64 i = 0; i < ((i64)width * (i64)height); ++i) {
        noise_map[i] = (noise_map[i] - min_h) / range;
    }
}