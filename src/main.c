#include "file_writer/file_writer.h"
#include "map/map.h"
#include <stdio.h>

int main() {
    MapInfo map_info;
    map_info.map_w = 1024;
    map_info.map_h = 1024;
    map_info.viewport_w = 1024;
    map_info.viewport_h = 1024;
    map_info.seed = 1331u;
    map_info.octaves = 5;
    map_info.scale = 140.0f;
    map_info.persistence = 0.5f;
    map_info.lacunarity = 2.0f;
    map_info.offset_x = 0.0f;
    map_info.offset_y = 0.0f;
    
    u8 *map = build_map(&map_info);
    u8 *image = (u8 *)calloc((size_t)(map_info.viewport_w * 
        map_info.viewport_h * 3), sizeof(u8));
    render_viewport(&map_info, map, image);
    
    bmp_writer("output/bmp/map.bmp", map_info.viewport_w, 
        map_info.viewport_h, image);
    
    free(image);
    free(map);
    
    return 0;
}