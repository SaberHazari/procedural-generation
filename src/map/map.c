#include "map.h"
#include <math.h>

static void generate_noise_map(MapInfo *map_info, u32 seed, f32 *noise_map) {
    if(map_info->scale <= 0.0f) { map_info->scale = 0.0001f; }
    if(map_info->octaves > MAX_OCTAVES) { 
        map_info->octaves = MAX_OCTAVES; 
    }
    
    f32 oct_x[map_info->octaves], oct_y[map_info->octaves];
    Prng prng;
    prng_seed(&prng, seed);
    
    for(i32 i = 0; i < map_info->octaves; ++i) {
        oct_x[i] = prng_range(&prng, -100000.0f, 100000.0f) + map_info->offset_x;
        oct_y[i] = prng_range(&prng, -100000.0f, 100000.0f) + map_info->offset_y;
    }
    
    f32 half_w = (f32)map_info->width * 0.5f;
    f32 half_h = (f32)map_info->height * 0.5f;
    f32 min_h = 1e30f, max_h = -1e30f;
    
    for(i32 y = 0; y < map_info->height; ++y) {
        for(i32 x = 0; x < map_info->width; ++x) {
            f32 amplitude = 1.0f, frequency = 1.0f, h = 0.0f;
            
            for(i32 i = 0; i < map_info->octaves; ++i) {
                f32 sx = ((f32)x - half_w) / map_info->scale * frequency + oct_x[i];
                f32 sy = ((f32)y - half_h) / map_info->scale * frequency + oct_y[i];
                h += perlin2d(sx, sy) * amplitude;
                amplitude *= map_info->persistence;
                frequency *= map_info->lacunarity;
            }
            noise_map[y * map_info->width + x] = h;
            if(h < min_h) { min_h = h; }
            if(h > max_h) { max_h = h; }
        }
    }
    
    f32 range = max_h - min_h;
    if(range < 1e-6f) { range = 1e-6f; }
    for(i32 i = 0; i < (map_info->width * map_info->height); ++i) {
        noise_map[i] = (noise_map[i] - min_h) / range;
    }
}

typedef enum {
    BIOME_DEEP_WATER = 0, BIOME_SHALLOW_WATER, BIOME_SAND, 
    BIOME_DESERT,         BIOME_PLAINS,        BIOME_SWAMP, 
    BIOME_DRY_HILLS,      BIOME_FOREST,        BIOME_RAINFOREST, 
    BIOME_BARE_ROCK,      BIOME_ROCKY,         BIOME_ALPINE, 
    BIOME_SNOW,           BIOME_COUNT
} BiomeId;

static const Vec3 BIOME_COLORS[BIOME_COUNT] = {
    [BIOME_DEEP_WATER] = {0.15f, 0.28f, 0.62f}, [BIOME_SHALLOW_WATER] = {0.26f, 0.46f, 0.83f}, 
    [BIOME_SAND]       = {0.86f, 0.80f, 0.55f}, [BIOME_DESERT]        = {0.80f, 0.66f, 0.36f}, 
    [BIOME_PLAINS]     = {0.38f, 0.66f, 0.26f}, [BIOME_SWAMP]         = {0.30f, 0.40f, 0.27f}, 
    [BIOME_DRY_HILLS]  = {0.58f, 0.55f, 0.33f}, [BIOME_FOREST]        = {0.29f, 0.53f, 0.21f}, 
    [BIOME_RAINFOREST] = {0.09f, 0.38f, 0.17f}, [BIOME_BARE_ROCK]     = {0.46f, 0.39f, 0.33f}, 
    [BIOME_ROCKY]      = {0.37f, 0.31f, 0.27f}, [BIOME_ALPINE]        = {0.47f, 0.52f, 0.50f}, 
    [BIOME_SNOW]       = {0.96f, 0.97f, 0.98f}, 
};

// rows for elevation, cols for moisture
static const BiomeId LAND_BIOME_TABLE[3][3] = {
    { BIOME_DESERT,    BIOME_PLAINS, BIOME_SWAMP      }, 
    { BIOME_DRY_HILLS, BIOME_FOREST, BIOME_RAINFOREST }, 
    { BIOME_BARE_ROCK, BIOME_ROCKY,  BIOME_ALPINE     }, 
};

static u8 classify_cell(f32 h, f32 m) {
    const f32 land_min = 0.48f, land_max = 0.92f;
    if(h <= 0.40f)    { return BIOME_DEEP_WATER;    }
    if(h <= 0.45f)    { return BIOME_SHALLOW_WATER; }
    if(h <= land_min) { return BIOME_SAND;          }
    if(h > land_max)  { return BIOME_SNOW;          }
    i32 elevation = (i32)(((h - land_min) / (land_max - land_min)) * 3);
    if(elevation > 2) { elevation = 2; }
    i32 moisture = (i32)(m * 3);
    if(moisture < 0) { moisture = 0; }
    if(moisture > 2) { moisture = 2; }
    return (u8)LAND_BIOME_TABLE[elevation][moisture];
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

u8 *build_map(MapInfo *map_info, MapSeeds *map_seeds) {
    f32 *height_map = (f32 *)calloc((size_t)(map_info->width * map_info->height), 
        sizeof(f32));
    f32 *moisture_map = (f32 *)calloc((size_t)(map_info->width * map_info->height), 
        sizeof(f32));
    generate_noise_map(map_info, map_seeds->height, height_map);
    generate_noise_map(map_info, map_seeds->moisture, moisture_map);
    u8 *map = (u8 *)calloc((size_t)(map_info->width * map_info->height), 
        sizeof(u8));
    for(i32 y = 0; y < map_info->height; ++y) {
        for(i32 x = 0; x < map_info->width; ++x) {
            i32 idx = y * map_info->width + x;
            f32 h = height_map[idx] - falloff_value(x, y, 
                map_info->width, map_info->height);
            if(h < 0.0f) { h = 0.0f; }
            if(h > 1.0f) { h = 1.0f; }
            f32 m = moisture_map[idx];
            if(m < 0.0f) { m = 0.0f; }
            if(m > 1.0f) { m = 1.0f; }
            map[idx] = classify_cell(h, m);
        }
    }
    
    free(height_map);
    height_map = NULL;
    free(moisture_map);
    moisture_map = NULL;
    return map;
}

static inline u8 color_f32_to_u8(f32 color) {
    u8 result = (u8)(color * 255.0f);
    return result;
}

void render_viewport(const MapInfo *map_info, 
    const Viewport *viewport, const u8 *map, u8 *image) {
    for(i32 y = 0; y < viewport->height; ++y) {
        for(i32 x = 0; x < viewport->width; ++x) {
            u8 cell = map[(viewport->offset_y + y) * 
                map_info->width + (viewport->offset_x + x)];
            Vec3 c = BIOME_COLORS[cell];
            i32 idx = (y * viewport->width + x) * 3;
            image[idx + 0] = color_f32_to_u8(c.x);
            image[idx + 1] = color_f32_to_u8(c.y);
            image[idx + 2] = color_f32_to_u8(c.z);
        }
    }
}