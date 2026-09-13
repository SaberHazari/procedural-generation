#ifndef MAP_H
#define MAP_H

#include "linalg/linalg.h"
#include "noise/noise.h"
#include "prng/prng.h"

#define MAX_OCTAVES 16

typedef struct MapInfo {
    i32 map_w;
    i32 map_h;
    i32 viewport_w;
    i32 viewport_h;
    u32 seed;
    i32 octaves;
    f32 scale;
    f32 persistence;
    f32 lacunarity;
    f32 offset_x;
    f32 offset_y;
} MapInfo;

typedef struct CellInfo {
    f32 height;
    Vec3 color;
} CellInfo;

static inline u8 color_f32_to_u8(f32 color) {
    u8 result = (u8)(color * 255.0f);
    return result;
}

u8 *build_map(MapInfo *map);
void render_viewport(MapInfo *map, const u8 *world_map, 
    u8 *image);

#endif // MAP_H