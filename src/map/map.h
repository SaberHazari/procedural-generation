#ifndef MAP_H
#define MAP_H

#include "linalg/linalg.h"
#include "noise/noise.h"
#include "prng/prng.h"
#include <stdlib.h>

#define MAX_OCTAVES 16

typedef struct MapSeeds {
    u32 height;
    u32 moisture;
} MapSeeds;

typedef struct MapInfo {
    i32 width;
    i32 height;
    i32 octaves;
    f32 scale;
    f32 persistence;
    f32 lacunarity;
    f32 offset_x;
    f32 offset_y;
} MapInfo;

typedef struct Viewport {
    i32 width;
    i32 height;
    i32 offset_x;
    i32 offset_y;
} Viewport;

u8 *build_map(MapInfo *map, MapSeeds *map_seeds);
void render_viewport(const MapInfo *map_info, 
    const Viewport *viewport, const u8 *map, u8 *image);

#endif // MAP_H